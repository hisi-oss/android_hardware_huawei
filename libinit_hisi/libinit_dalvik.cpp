/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "libinit_dalvik"

#include <libinit_dalvik.h>
#include <libinit_utils.h>

#include <android-base/logging.h>

#include <sys/sysinfo.h>

#define HEAPSTARTSIZE_PROP "dalvik.vm.heapstartsize"
#define HEAPGROWTHLIMIT_PROP "dalvik.vm.heapgrowthlimit"
#define HEAPSIZE_PROP "dalvik.vm.heapsize"
#define HEAPMINFREE_PROP "dalvik.vm.heapminfree"
#define HEAPMAXFREE_PROP "dalvik.vm.heapmaxfree"
#define HEAPTARGETUTILIZATION_PROP "dalvik.vm.heaptargetutilization"

#define GB(b) (b * 1024ull * 1024 * 1024)

static const dalvik_heap_info_t dalvik_heap_info_6144 = {
        .heapstartsize = "16m",
        .heapgrowthlimit = "256m",
        .heapsize = "512m",
        .heapminfree = "8m",
        .heapmaxfree = "32m",
        .heaptargetutilization = "0.5",
};

static const dalvik_heap_info_t dalvik_heap_info_4096 = {
        .heapstartsize = "8m",
        .heapgrowthlimit = "256m",
        .heapsize = "512m",
        .heapminfree = "8m",
        .heapmaxfree = "16m",
        .heaptargetutilization = "0.6",
};

static const dalvik_heap_info_t dalvik_heap_info_2048 = {
        .heapstartsize = "8m",
        .heapgrowthlimit = "192m",
        .heapsize = "512m",
        .heapminfree = "512k",
        .heapmaxfree = "8m",
        .heaptargetutilization = "0.75",
};

void load_dalvik() {
    struct sysinfo sys;
    const dalvik_heap_info_t* dhi;

    sysinfo(&sys);

    if (sys.totalram > GB(5)) {
        LOG(INFO) << "Setting dalvik props for >6gb devices";
        dhi = &dalvik_heap_info_6144;
    } else if (sys.totalram > GB(3)) {
        LOG(INFO) << "Setting dalvik props for >4gb devices";
        dhi = &dalvik_heap_info_4096;
    } else {
        LOG(INFO) << "Setting dalvik props for <3gb devices";
        dhi = &dalvik_heap_info_2048;
    }

    property_override(HEAPSTARTSIZE_PROP, dhi->heapstartsize);
    property_override(HEAPGROWTHLIMIT_PROP, dhi->heapgrowthlimit);
    property_override(HEAPSIZE_PROP, dhi->heapsize);
    property_override(HEAPTARGETUTILIZATION_PROP, dhi->heaptargetutilization);
    property_override(HEAPMINFREE_PROP, dhi->heapminfree);
    property_override(HEAPMAXFREE_PROP, dhi->heapmaxfree);
}
