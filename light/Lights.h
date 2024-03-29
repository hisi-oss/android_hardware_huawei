/*
 * Copyright (C) 2021-2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <aidl/android/hardware/light/BnLights.h>
#include <mutex>
#include "Backlight.h"
#include "Button.h"

using ::aidl::android::hardware::light::HwLight;
using ::aidl::android::hardware::light::HwLightState;

namespace aidl {
namespace android {
namespace hardware {
namespace light {

class Lights : public BnLights {
  public:
    Lights();

    ndk::ScopedAStatus setLightState(int32_t id, const HwLightState& state) override;
    ndk::ScopedAStatus getLights(std::vector<HwLight>* _aidl_return) override;

  private:
    void setLED();

    std::vector<HwLight> mLights;

    BacklightDevice* mBacklightDevice;
    ButtonDevice* mButtonDevice;
    bool mWhiteLED;

    std::mutex mLEDMutex;

    HwLightState mLastBatteryState;
    HwLightState mLastNotificationState;
    HwLightState mLastAttentionState;
};

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl
