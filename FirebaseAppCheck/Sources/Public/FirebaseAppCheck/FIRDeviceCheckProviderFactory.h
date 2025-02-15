/*
 * Copyright 2020 Google LLC
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

#import <Foundation/Foundation.h>

#import "FIRAppCheckProviderFactory.h"

NS_ASSUME_NONNULL_BEGIN

/// An implementation of `AppCheckProviderFactory` that creates a new instance of
/// `DeviceCheckProvider` for the specified `FirebaseApp` on request. Currently
/// `DeviceCheckProviderFactory` is the default that will be used by Firebase app check if no other
/// provider is specified. See `AppCheck` class for more details.
API_AVAILABLE(ios(11.0), macos(10.15), tvos(11.0))
API_UNAVAILABLE(watchos)
NS_SWIFT_NAME(DeviceCheckProviderFactory)
@interface FIRDeviceCheckProviderFactory : NSObject <FIRAppCheckProviderFactory>

@end

NS_ASSUME_NONNULL_END
