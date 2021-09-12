/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aidl/android/hardware/ir/BnConsumerIr.h>
#include <android-base/logging.h>
#include <android/binder_interface_utils.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <numeric>

namespace aidl::android::hardware::ir {

const std::vector<ConsumerIrFreqRange> kSupportedFreqs = {
        {2000, 4000},
        {10000, 30000},
};

class ConsumerIr : public BnConsumerIr {
    ::ndk::ScopedAStatus getCarrierFreqs(std::vector<ConsumerIrFreqRange>* _aidl_return) override;
    ::ndk::ScopedAStatus transmit(int32_t in_carrierFreqHz,
                                  const std::vector<int32_t>& in_pattern) override;
};

::ndk::ScopedAStatus ConsumerIr::getCarrierFreqs(std::vector<ConsumerIrFreqRange>* _aidl_return) {
    *_aidl_return = kSupportedFreqs;
    return ::ndk::ScopedAStatus::ok();
}

bool isSupportedFreq(int32_t freq) {
    for (const auto& range : kSupportedFreqs) {
        if (freq >= range.minHz && freq <= range.maxHz) return true;
    }
    return false;
}

::ndk::ScopedAStatus ConsumerIr::transmit(int32_t in_carrierFreqHz,
                                          const std::vector<int32_t>& in_pattern) {
    if (isSupportedFreq(in_carrierFreqHz)) {
        // trasmit the pattern, each integer is number of microseconds in an
        // alternating on/off state.
        usleep(std::accumulate(in_pattern.begin(), in_pattern.end(), 0));
        return ::ndk::ScopedAStatus::ok();
    } else {
        // unsupported operation
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }
    return ::ndk::ScopedAStatus::ok();
}

}  // namespace aidl::android::hardware::ir

using aidl::android::hardware::ir::ConsumerIr;

int main() {
    auto binder = ::ndk::SharedRefBase::make<ConsumerIr>();
    const std::string name = std::string() + ConsumerIr::descriptor + "/default";
    CHECK_EQ(STATUS_OK, AServiceManager_addService(binder->asBinder().get(), name.c_str()))
            << "Failed to register " << name;

    ABinderProcess_setThreadPoolMaxThreadCount(0);
    ABinderProcess_joinThreadPool();

    return EXIT_FAILURE;  // should not reached
}
