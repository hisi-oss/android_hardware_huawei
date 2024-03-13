/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "hisi_connectivity"

#include "include/hisi_connectivity.h"
#include "include/hisi_utils.h"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>

#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <string>

constexpr const char* kChiptypePath = "/proc/connectivity/chiptype";
constexpr const char* kDeviceTreePath = "/proc/device-tree";
constexpr const char* kPropSubChipType = "ro.connectivity.sub_chiptype";
constexpr const char* kPropChipType = "ro.connectivity.chiptype";

constexpr const char* kCmdline = "/proc/cmdline";
constexpr const char* kDefaultId = "0X00000000";
constexpr const char* kPropRilReady = "sys.rilprops_ready";

// clang-format off
constexpr const char* kDenylistedProperties[] = {
    "ro.telephony.default_network"
};

constexpr const char* kPhonePropPaths[] = {
    "/vendor/phone.prop",
    "/odm/phone.prop"
};
// clang-format on

std::string ReadProductId() {
    std::string prid = kDefaultId;
    std::string cmdline;

    if (android::base::ReadFileToString(kCmdline, &cmdline)) {
        for (const auto& i : android::base::Split(android::base::Trim(cmdline), " ")) {
            std::vector<std::string> parts = android::base::Split(i, "=");
            if (parts.size() == 2 && parts.at(0) == "productid") {
                prid = parts.at(1);
                std::transform(prid.begin(), prid.end(), prid.begin(),
                               [](unsigned char c) { return toupper(c); });
            }
        }
    }

    return prid;
}

static int SetPhoneProperties(std::string prid, std::string propFile) {
    int ret = -1;
    std::string line;
    std::ifstream file(propFile);

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (ret == 0 && line.length() == 0) break;
            if (line.find(prid) != std::string::npos) ret = 0;

            if (ret == 0) {
                std::vector<std::string> parts = android::base::Split(line, "=");
                if (parts.size() == 2) {
                    if (std::find(std::begin(kDenylistedProperties),
                                  std::end(kDenylistedProperties),
                                  parts.at(0)) == std::end(kDenylistedProperties)) {
                        set_property(parts.at(0), parts.at(1));
                    }
                }
            }
        }
    }

    return ret;
}

static int LoadPhoneProperties() {
    int ret = -1;

    std::string productId = ReadProductId();
    if (productId != kDefaultId) {
        for (const auto& path : kPhonePropPaths) {
            if ((ret = SetPhoneProperties(productId, path)) == 0) {
                LOG(INFO) << "Successfully loaded phone properties ( " << path << ") for "
                          << productId;
                set_property(kPropRilReady, "1");
                return ret;
            }
        }
    }

    return ret;
}

static int LoadChipProperties() {
    int ret = -1;
    std::string chip_type;
    std::string subchip_path;

    // This is the main chip type, and it can be used to determine the hardware
    // revision. In our case, we can have either hisi or bcm.
    if (!android::base::ReadFileToString(kChiptypePath, &chip_type)) {
        LOG(ERROR) << "Unable to read: " << kChiptypePath;
        return ret;
    }

    // Set the property, so that the init scripts can be included conditionally.
    set_property(kPropChipType, chip_type);

    // This is the subchip type, and it may be different depending on the hardware
    // revision. In our case, we can have either hi11xx or bcm43xx.
    if (chip_type.find("hisi") == 0) {
        subchip_path = std::string(kDeviceTreePath) + "/hi110x/hi110x,subchip_type";
        if (access(subchip_path.c_str(), F_OK) != 0) {
            subchip_path = std::string(kDeviceTreePath) + "/hi1102/name";
        }
    } else {
        subchip_path = std::string(kDeviceTreePath) + "/bcm_wifi/ic_type";
    }

    if (!android::base::ReadFileToString(subchip_path, &chip_type)) {
        LOG(ERROR) << "Unable to determine a valid subchip type";
        return ret;
    }

    // Set the property, so that the init scripts can be included conditionally.
    set_property(kPropSubChipType, chip_type);

    return 0;
}

void load_hisi_connectivity() {
    if (LoadChipProperties() < 0) LOG(WARNING) << "Unable to load chip properties";

    if (LoadPhoneProperties() < 0) LOG(WARNING) << "Unable to load phone properties";
}
