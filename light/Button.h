/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Utils.h"

namespace aidl {
namespace android {
namespace hardware {
namespace light {

class ButtonDevice {
  public:
    virtual ~ButtonDevice() = default;

    virtual void setButton(uint8_t value) = 0;
    virtual bool exists() = 0;
};

ButtonDevice* getButtonDevice();

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl
