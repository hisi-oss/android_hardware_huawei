/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "hisi_utils"

#include "include/hisi_utils.h"

#include <android-base/logging.h>
#include <android-base/properties.h>

#include <string>
#include <vector>

void set_property(const std::string& prop, const std::string& value) {
    LOG(INFO) << "Setting property: " << prop << " to " << value;

    if (!android::base::SetProperty(prop, value)) {
        LOG(ERROR) << "Unable to set: " << prop << " to " << value;
    }
}
