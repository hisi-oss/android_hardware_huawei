/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "hisi_nve"

#include "include/hisi_nve.h"

#include <android-base/logging.h>
#include <android-base/properties.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value << std::endl;
}

std::string parse_mac(const std::string& mac) {
    std::string result;

    result += mac.substr(0, 2) + ':';
    result += mac.substr(2, 2) + ':';
    result += mac.substr(4, 2) + ':';
    result += mac.substr(6, 2) + ':';
    result += mac.substr(8, 2) + ':';
    result += mac.substr(10, 2);

    return result;
}

std::string load_nve_path() {
    // Loop over all the possible paths and use
    // the first one that exists and can be read.
    for (const auto& path : kNvePaths) {
        // Make sure the path is accessible for us.
        if (access(path.c_str(), R_OK) == 0) return path;
    }

    // If we reached this point, we couldn't find
    // a suitable path. Return an empty string and
    // let the caller handle the situation.
    return "";
}

std::streampos find_start_offset(std::ifstream& file) {
    if (!file.is_open()) return std::streampos(-1);

    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    const size_t bufferSize = 4096;
    char buffer[bufferSize];

    for (std::streampos i = 0; i < file_size; i += bufferSize - 7) {
        file.seekg(i);
        file.read(buffer, bufferSize);
        for (size_t j = 0; j < bufferSize - 7; ++j) {
            if (std::memcmp(buffer + j, "SWVERSI", 7) == 0) {
                return i + static_cast<std::streampos>(j);
            }
        }
    }
    return std::streampos(-1);
}

std::string nve_read(const std::string& name, const std::string& path) {
    // The caller is supposed to pass the path to the NVE
    // partition block and the name of the entry to read.
    nv_item entry;
    std::ifstream file(path, std::ios::binary);

    // This shouldn't be required but make sure the path
    // the caller passed is valid and the file can be read.
    if (!file.is_open()) {
        // If we can't open the file, return an empty string
        // and let the caller handle the situation.
        LOG(ERROR) << "Unable to open " << path;
        return "";
    }

    // The next step is to find the start offset of the NVE
    // partition. This is done by searching for the "SWVERSI"
    // string and then going back 4 bytes.
    std::streampos start_offset = find_start_offset(file);
    if (start_offset == std::streampos(-1)) {
        // If we can't find the start offset, return an empty
        // string and let the caller handle the situation.
        LOG(ERROR) << "Unable to find the start offset of the NVE partition";
        return "";
    }

    // Now that we know the start offset, we can seek to it
    // and start reading the entries.
    file.seekg(start_offset - static_cast<std::streamoff>(4));
    while (file.read(reinterpret_cast<char*>(&entry), sizeof(nv_item))) {
        if (std::memcmp(entry.nv_name, name.c_str(), name.length()) == 0) {
            // If the entry name starts with "MAC", we need to add
            // a colon between each byte of the MAC address.
            return std::string(entry.nv_data, entry.valid_size);
        }
    }

    // If we reached this point, we couldn't find the entry
    // the caller was looking for. Return an empty string
    // and let the caller handle the situation.
    return "";
}

int LoadNveProperties() {
    std::string path = load_nve_path();
    if (path.empty()) {
        LOG(ERROR) << "Unable to find a suitable path for the NVE partition";
        return -1;
    }

    for (const auto& pair : kNveMacMap) {
        if (std::string mac_value = nve_read(pair.first, path); mac_value.empty()) {
            LOG(WARNING) << "Unable to read " << pair.first << " from NVE partition";
        } else {
            set(pair.second, parse_mac(mac_value));
        }
    }

    android::base::SetProperty(kPropMacsAreReady, "1");
    return 0;
}

void load_hisi_nve() {
    if (LoadNveProperties() < 0) LOG(WARNING) << "Unable to load nve properties";
}
