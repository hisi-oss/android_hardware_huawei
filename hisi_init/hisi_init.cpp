/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "hisi_init"

#include "include/hisi_connectivity.h"

#include <android-base/logging.h>

int main() {
    LOG(INFO) << "Running hisi_connectivity";
    load_hisi_connectivity();
}
