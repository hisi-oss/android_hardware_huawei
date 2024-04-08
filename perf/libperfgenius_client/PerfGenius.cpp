/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "libperfgenius_client"

#include <android-base/logging.h>
#include <vector>

#include "PerfGenius.h"

int perfRequest(uint32_t dur, [[maybe_unused]] const std::vector<ClientPerfRequestInfo>& requests,
                [[maybe_unused]] unsigned long& outParam) {
    LOG(INFO) << __func__ << ": dur: " << dur;
    return 0;
}

int perfRequest(unsigned int hdl,
                [[maybe_unused]] const std::vector<ClientPerfRequestInfo>& requests,
                [[maybe_unused]] unsigned long long& outParam) {
    LOG(INFO) << __func__ << ": hdl: " << hdl;
    return 0;
}

int perfRelease(unsigned long hdl) {
    LOG(INFO) << __func__ << ": hdl: " << hdl;
    return 0;
}

int perfRelease(unsigned long long hdl) {
    LOG(INFO) << __func__ << ": hdl: " << hdl;
    return 0;
}

int perfRelease(unsigned long hdl1, unsigned long hdl2) {
    LOG(INFO) << __func__ << ": hdl1: " << hdl1 << "; hdl2: " << hdl2;
    return 0;
}
