/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string>
#include <utility>
#include <vector>

constexpr const char* kPropMacsAreReady = "sys.conn_macs.ready";

enum nv_operation {
    NV_WRITE = 0,
    NV_READ = 1,
};

#pragma pack(push, 1)
typedef struct nv_item {
    unsigned int nv_number;
    char nv_name[8];
    unsigned int nv_property;
    unsigned int valid_size;
    unsigned int crc;
    char nv_data[104];
} nv_item;
#pragma pack(pop)

const std::pair<std::string, std::string> kNveMacMap[] = {
        {"MACWLAN", "/data/vendor/wifi/macwlan"},
        {"MACBT", "/data/vendor/bluedroid/macbt"},
};

const std::vector<std::string> kNvePaths = {
        "/dev/block/by-name/nvme",
        "/dev/block/platform/hi_mci.0/by-name/nvme",
        "/dev/block/mmcblk0p7",
};

void load_hisi_nve();
