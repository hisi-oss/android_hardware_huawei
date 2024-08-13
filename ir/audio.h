/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <android/hardware/audio/4.0/IDevice.h>
#include <android/hardware/audio/4.0/IDevicesFactory.h>
#include <android/hardware/audio/5.0/IDevice.h>
#include <android/hardware/audio/5.0/IDevicesFactory.h>

using ::android::sp;

namespace aidl::android::hardware::ir {

class AudioService {
  public:
    AudioService();
    void initializeAudioService();
    void getIrdaSupportParameter();
    void setTransmitParameter(bool start);

  private:
    sp<::android::hardware::audio::V5_0::IDevice> mAudioDevice_V5_0;
    sp<::android::hardware::audio::V5_0::IDevicesFactory> mAudioDevicesFactory_V5_0;
    sp<::android::hardware::audio::V4_0::IDevice> mAudioDevice_V4_0;
    sp<::android::hardware::audio::V4_0::IDevicesFactory> mAudioDevicesFactory_V4_0;

    std::string mParameter;
};

}  // namespace aidl::android::hardware::ir
