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

#import "FIRAppCheckProvider.h"

@class FIRApp;
@protocol FIRDeviceCheckAPIServiceProtocol;
@protocol FIRDeviceCheckTokenGenerator;

NS_ASSUME_NONNULL_BEGIN

/// Firebase app check provider that verifies the app integrity using
/// [DeviceCheck](https://developer.apple.com/documentation/devicecheck) API.
API_AVAILABLE(ios(11.0), macos(10.15), tvos(11.0))
API_UNAVAILABLE(watchos)
NS_SWIFT_NAME(DeviceCheckProvider)
@interface FIRDeviceCheckProvider : NSObject <FIRAppCheckProvider>

- (instancetype)init NS_UNAVAILABLE;

/// The default initializer.
/// @param app A `FirebaseApp` instance.
/// @return An instance of `DeviceCheckProvider` if the provided `FirebaseApp` instance contains all
/// required parameters.
- (nullable instancetype)initWithApp:(FIRApp *)app;

@end

NS_ASSUME_NONNULL_END
