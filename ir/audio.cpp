/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "audio.h"

#include <android-base/logging.h>

#include <log/log.h>

using ::android::sp;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;

namespace aidl::android::hardware::ir {

AudioService::AudioService() {
    mAudioDevicesFactory_V5_0 = ::android::hardware::audio::V5_0::IDevicesFactory::getService();
    if (mAudioDevicesFactory_V5_0 == nullptr) {
        LOG(INFO) << "mAudioDevicesFactory_V5_0 null, trying V4_0";
        mAudioDevicesFactory_V4_0 = ::android::hardware::audio::V4_0::IDevicesFactory::getService();

        if (mAudioDevicesFactory_V4_0 == nullptr) {
            LOG(INFO) << "mAudioDevicesFactory_V4_0 null, aborting";
            return;
        }
    }
}

void AudioService::initializeAudioService() {
    if (mAudioDevicesFactory_V5_0 != nullptr) {
        auto openDevice_cb =
                std::function<void(::android::hardware::audio::V5_0::Result,
                                   const sp<::android::hardware::audio::V5_0::IDevice>&)>(
                        [this](::android::hardware::audio::V5_0::Result result,
                               const sp<::android::hardware::audio::V5_0::IDevice>& device) {
                            if (result == ::android::hardware::audio::V5_0::Result::OK) {
                                this->mAudioDevice_V5_0 = device;
                            } else {
                                LOG(INFO) << "Could not open primary audio device";
                            }
                        });
        mAudioDevicesFactory_V5_0->openDevice("primary", openDevice_cb);

        if (mAudioDevice_V5_0 == nullptr) {
            LOG(INFO) << "mAudioDevice_V5_0 is null, aborting.";
            return;
        }
    } else if (mAudioDevicesFactory_V4_0 != nullptr) {
        auto openDevice_cb =
                std::function<void(::android::hardware::audio::V4_0::Result,
                                   const sp<::android::hardware::audio::V4_0::IDevice>&)>(
                        [this](::android::hardware::audio::V4_0::Result result,
                               const sp<::android::hardware::audio::V4_0::IDevice>& device) {
                            if (result == ::android::hardware::audio::V4_0::Result::OK) {
                                this->mAudioDevice_V4_0 = device;
                            } else {
                                LOG(INFO) << "Could not open primary audio device";
                            }
                        });
        mAudioDevicesFactory_V4_0->openDevice("primary", openDevice_cb);

        if (mAudioDevice_V4_0 == nullptr) {
            LOG(INFO) << "mAudioDevice_V4_0 is null, aborting.";
            return;
        }
    }
}

void AudioService::getIrdaSupportParameter() {
    hidl_vec<hidl_string> keys = {"audio_capability#irda_support"};

    if (mAudioDevice_V5_0 != nullptr) {
        mAudioDevice_V5_0
                ->getParameters(
                        {}, keys,
                        [this](::android::hardware::audio::V5_0::Result retval,
                               const hidl_vec<::android::hardware::audio::V5_0::ParameterValue>&
                                       parameters) {
                            if (retval == ::android::hardware::audio::V5_0::Result::OK &&
                                parameters.size() > 0) {
                                this->mParameter = parameters[0].key;
                            } else {
                                this->mParameter.clear();
                            }
                        })
                .isOk();
    } else if (mAudioDevice_V4_0 != nullptr) {
        mAudioDevice_V4_0
                ->getParameters(
                        {}, keys,
                        [this](::android::hardware::audio::V4_0::Result retval,
                               const hidl_vec<::android::hardware::audio::V4_0::ParameterValue>&
                                       parameters) {
                            if (retval == ::android::hardware::audio::V4_0::Result::OK &&
                                parameters.size() > 0) {
                                this->mParameter = parameters[0].key;
                            } else {
                                this->mParameter.clear();
                            }
                        })
                .isOk();
    }
}

void AudioService::setTransmitParameter(bool start) {
    if (!mParameter.empty() && mParameter.find("true") != std::string::npos) {
        if (mAudioDevice_V5_0 != nullptr) {
            hidl_vec<::android::hardware::audio::V5_0::ParameterValue> parameters = {
                    {"ir_trans", start ? "on" : "off"}};
            mAudioDevice_V5_0->setParameters({}, parameters);
        } else if (mAudioDevice_V4_0 != nullptr) {
            hidl_vec<::android::hardware::audio::V4_0::ParameterValue> parameters = {
                    {"ir_trans", start ? "on" : "off"}};
            mAudioDevice_V4_0->setParameters({}, parameters);
        }
    }
}

}  // namespace aidl::android::hardware::ir
