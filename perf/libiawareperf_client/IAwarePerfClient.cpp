/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "libiawareperf_client.stub"

#include <aidl/android/hardware/power/Boost.h>
#include <aidl/android/hardware/power/IPower.h>
#include <android-base/logging.h>
#include <android/binder_manager.h>

#include "IAwarePerfClient.h"

using ::aidl::android::hardware::power::Boost;

static constexpr int kDefaultBoostDurationMs = 1000;

static std::shared_ptr<aidl::android::hardware::power::IPower> gAidlPowerHal;
static const std::string kInstance =
        std::string(aidl::android::hardware::power::IPower::descriptor) + "/default";

bool getAidlPowerHal(void) {
    if (!gAidlPowerHal) {
        ndk::SpAIBinder pwBinder = ndk::SpAIBinder(AServiceManager_getService(kInstance.c_str()));
        gAidlPowerHal = aidl::android::hardware::power::IPower::fromBinder(pwBinder);
    }

    return !!gAidlPowerHal;
}

extern "C" void uniPerfEvent(int eventId, char* pkgName, int payloadNum,
                             [[maybe_unused]] int* payload) {
    if (!getAidlPowerHal()) {
        LOG(ERROR) << "Can't get AIDL Power HAL!";
    } else {
        LOG(VERBOSE) << "Connected to power AIDL HAL";
    }

    LOG(DEBUG) << "eventId: " << eventId << ", pkgName: " << pkgName
               << ", payloadNum: " << payloadNum;

    switch (eventId) {
        case UNIPERF_EVENT_FINGERPRINT:
            gAidlPowerHal->setBoost(Boost::INTERACTION, kDefaultBoostDurationMs);
            break;
    }
}
