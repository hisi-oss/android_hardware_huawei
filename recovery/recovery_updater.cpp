/*
 * Copyright (C) 2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/properties.h>
#include <edify/expr.h>
#include <liblp/liblp.h>
#include <otautil/error_code.h>

Value* RetrofitDynamicPartitions(const char* /* name */, State* state,
                                 const std::vector<std::unique_ptr<Expr>>& /* argv */) {
    int ret = 0;
    std::unique_ptr<android::fs_mgr::LpMetadata> pt;
    std::string super_partition = android::base::GetProperty("ro.boot.super_partition", "");

    if (super_partition.empty()) {
        return ErrorAbort(
                state, kVendorFailure,
                "The current recovery does not seem to support dynamic partitions, please update.");
    }

    pt = android::fs_mgr::ReadMetadata(super_partition, 0);
    if (!pt) {  // No dynamic partitions metadata yet - flashing initial metadata is required
        state->updater->UiPrint("Flashing initial dynamic partitions partition table...");

        pt = android::fs_mgr::ReadFromImageFile("/tmp/super_empty.img");
        if (!pt) {
            return ErrorAbort(state, kFileOpenFailure, "Failed to read /tmp/super_empty.img");
        }

        if (!android::fs_mgr::FlashPartitionTable(super_partition, *pt.get())) {
            return ErrorAbort(state, kSetMetadataFailure,
                              "Failed to flash partition table of super partition: %s.",
                              super_partition.c_str());
        }
    }

    return StringValue(std::to_string(ret));
}

void Register_librecovery_updater_huawei() {
    RegisterFunction("huawei.retrofit_dynamic_partitions", RetrofitDynamicPartitions);
}
