/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "libinit_variants"
#include <libinit_utils.h>
#include <libinit_variants.h>

#include <android-base/logging.h>
#include <android-base/strings.h>

#include <sstream>

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

struct ProductInfo {
    std::string model;
    std::string version;
    std::string region_type;
};

constexpr const char* kOemInfoPath = "/dev/block/by-name/oeminfo";
std::vector<unsigned char> pattern = {0x4F, 0x45, 0x4D, 0x5F, 0x49, 0x4E, 0x46, 0x4F, 0x06, 0x00,
                                      0x00, 0x00, 0x4E, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                                      0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};

ProductInfo ParseProductInfo(const std::string& product_info_str) {
    ProductInfo product_info;
    std::istringstream iss(product_info_str);

    // Extract the model (i.e. "PRA-LX1").
    std::getline(iss, product_info.model, ' ');

    // Extract the version (i.e. "9.1.0.311") including parentheses if present.
    std::getline(iss, product_info.version, '(');

    // Remove trailing whitespace.
    android::base::Trim(product_info.version);

    // If version is enclosed in parentheses, extract the region_type.
    if (!product_info.version.empty() && product_info.version.back() == ')') {
        product_info.version.pop_back();

        // Extract the region_type (i.e. "C185E3R2P1").
        std::getline(iss, product_info.region_type, ')');

        // Trim leading and trailing whitespaces from region_type.
        android::base::Trim(product_info.region_type);
    }

    return product_info;
}

ProductInfo ReadProductInfo() {
    ProductInfo product_info = {};

    int fd = open(kOemInfoPath, O_RDONLY);
    if (fd == -1) {
        LOG(ERROR) << "Unable to open: " << kOemInfoPath << ", error: " << strerror(errno);
        return product_info;
    }

    off_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);  // Go back.

    void* buffer = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buffer == MAP_FAILED) {
        LOG(ERROR) << "Unable to map: " << kOemInfoPath << ", error: " << strerror(errno);
        close(fd);
        return product_info;
    }

    auto begin = static_cast<unsigned char*>(buffer);
    auto end = begin + size;
    auto it = std::search(begin, end, pattern.begin(), pattern.end());

    if (it != end) {
        // Skip over 0xFF bytes
        auto name_start = it + pattern.size();
        while (*name_start == 0xFF && name_start < end) {
            ++name_start;
        }
        // Parse the product info
        product_info = ParseProductInfo(std::string(reinterpret_cast<char*>(name_start)));
    } else {
        LOG(ERROR) << "Unable to find product name in: " << kOemInfoPath;
    }

    munmap(buffer, size);
    close(fd);

    return product_info;
}

void load_variants() {
    ProductInfo product_info = ReadProductInfo();

    // Load the phone model dynamically from the oeminfo partition.
    if (!product_info.model.empty()) {
        LOG(INFO) << "Found product info: " << product_info.model << " " << product_info.version
                  << " " << product_info.region_type;
        set_ro_build_prop("model", product_info.model, true);
    } else {
        LOG(ERROR) << "Unable to parse product information!";
    }
}
