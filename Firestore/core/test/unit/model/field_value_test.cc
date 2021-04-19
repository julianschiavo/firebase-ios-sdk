/*
 * Copyright 2018 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Firestore/core/src/model/value_util.h"

#if __APPLE__
#import <CoreFoundation/CoreFoundation.h>
#endif  // __APPLE__

#include <chrono>  // NOLINT(build/c++11)
#include <climits>
#include <cmath>
#include <vector>

#include "Firestore/core/src/model/field_mask.h"
#include "Firestore/core/src/model/field_path.h"
#include "Firestore/core/src/nanopb/byte_string.h"
#include "Firestore/core/src/util/defer.h"
#include "Firestore/core/src/util/string_apple.h"
#include "Firestore/core/test/unit/testutil/equals_tester.h"
#include "Firestore/core/test/unit/testutil/testutil.h"
#include "Firestore/core/test/unit/testutil/time_testing.h"
#include "absl/base/casts.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace firebase {
namespace firestore {
namespace model {

using absl::nullopt;
using nanopb::ByteString;
using testing::Not;
using testutil::Array;
using testutil::BlobValue;
using testutil::DbId;
using testutil::Field;
using testutil::Key;
using testutil::Map;
using testutil::time_point;
using testutil::Value;
using testutil::WrapObject;

using Sec = std::chrono::seconds;
using Ms = std::chrono::milliseconds;

namespace {

uint64_t ToBits(double value) {
  return absl::bit_cast<uint64_t>(value);
}

double ToDouble(uint64_t value) {
  return absl::bit_cast<double>(value);
}

// All permutations of the 51 other non-MSB significand bits are also NaNs.
const uint64_t kAlternateNanBits = 0x7fff000000000000ULL;

MATCHER(IsNan, "a NaN") {
  return std::isnan(arg);
}

}  // namespace

TEST(FieldValueTest, ValueHelpers) {
  // Validates that the Value helpers in testutil produce the right types
  google_firestore_v1_Value bool_value = Value(true);
  ASSERT_EQ(GetTypeOrder(bool_value), TypeOrder::kBoolean);
  EXPECT_EQ(bool_value.boolean_value, true);

  google_firestore_v1_Value int_value = Value(5);
  ASSERT_EQ(GetTypeOrder(int_value), TypeOrder::kNumber);
  EXPECT_EQ(int_value.integer_value, 5);

  google_firestore_v1_Value long_value = Value(LONG_MAX);
  ASSERT_EQ(GetTypeOrder(long_value), TypeOrder::kNumber);
  EXPECT_EQ(long_value.integer_value, LONG_MAX);

  google_firestore_v1_Value long_long_value = Value(LLONG_MAX);
  ASSERT_EQ(GetTypeOrder(long_long_value), TypeOrder::kNumber);
  EXPECT_EQ(long_long_value.integer_value, LLONG_MAX);

  google_firestore_v1_Value double_value = Value(2.0);
  ASSERT_EQ(GetTypeOrder(double_value), TypeOrder::kNumber);
  EXPECT_EQ(double_value.double_value, 2.0);
}

#if __APPLE__
// Validates that NSNumber/CFNumber normalize NaNs to the same values that
// Firestore does. This uses CoreFoundation's CFNumber instead of NSNumber just
// to keep the test in a single file.
TEST(FieldValueTest, CanonicalBitsAreCanonical) {
  double input = ToDouble(kAlternateNanBits);
  CFNumberRef number = CFNumberCreate(nullptr, kCFNumberDoubleType, &input);
  util::Defer cleanup([&] { util::SafeCFRelease(number); });

  double actual = 0.0;
  CFNumberGetValue(number, kCFNumberDoubleType, &actual);

  ASSERT_EQ(kCanonicalNanBits, ToBits(actual));
}
#endif  // __APPLE__

TEST(FieldValueTest, NormalizesNaNs) {
  // NOTE: With v1 query semantics, it's no longer as important that our NaN
  // representation matches the backend, since all NaNs are defined to sort as
  // equal, but we preserve the normalization and this test regardless for now.

  // Bedrock assumption: our canonical NaN bits are actually a NaN.
  double canonical = ToDouble(kCanonicalNanBits);
  double alternate = ToDouble(kAlternateNanBits);
  ASSERT_THAT(canonical, IsNan());
  ASSERT_THAT(alternate, IsNan());
  ASSERT_THAT(0.0, Not(IsNan()));

  // Round trip otherwise preserves NaNs
  EXPECT_EQ(kAlternateNanBits, ToBits(alternate));
  EXPECT_NE(kCanonicalNanBits, ToBits(alternate));

  // Creating a FieldValue from a double should normalize NaNs.
  auto Normalize = [](uint64_t bits) -> uint64_t {
    return ToBits(ToDouble(bits));
  };

  EXPECT_EQ(kCanonicalNanBits, Normalize(kAlternateNanBits));

  // A NaN that's canonical except it has the sign bit set (would be negative if
  // signs mattered)
  EXPECT_EQ(kCanonicalNanBits, Normalize(0xfff8000000000000ULL));

  // A signaling NaN with significand where MSB is 0, and some non-MSB bit is
  // one.
  EXPECT_EQ(kCanonicalNanBits, Normalize(0xfff4000000000000ULL));
}

}  // namespace model
}  // namespace firestore
}  // namespace firebase
