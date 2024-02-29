/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "libbt-vendor"

#include <dlfcn.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <string>

#include <cutils/properties.h>
#include <log/log.h>

#include "bt_hci_bdroid.h"
#include "bt_vendor_lib.h"

/******************************************************************************
**  Variables
******************************************************************************/

#define BCM_LIB_NAME "libbt-vendor-bcm.so"
#define HISI_LIB_NAME "libbt-vendor-hisi.so"

#define VENDOR_LIBRARY_SYMBOL_NAME "BLUETOOTH_VENDOR_LIB_INTERFACE"

static void* lib_handle = nullptr;
bt_vendor_interface_t* lib_interface = nullptr;

/******************************************************************************
**  Functions
******************************************************************************/

static int load_vendor_library(const std::string& lib_name) {
    lib_handle = dlopen(lib_name.c_str(), RTLD_LAZY);
    if (!lib_handle) {
        ALOGE("Failed to load %s: %s", lib_name.c_str(), dlerror());
        return -1;
    }

    lib_interface =
            reinterpret_cast<bt_vendor_interface_t*>(dlsym(lib_handle, VENDOR_LIBRARY_SYMBOL_NAME));

    if (!lib_interface) {
        ALOGE("Failed to find required symbol (%s) in %s: %s", VENDOR_LIBRARY_SYMBOL_NAME,
              lib_name.c_str(), dlerror());
        dlclose(lib_handle);
        lib_handle = nullptr;
        return -1;
    }

    return 0;
}

/*****************************************************************************
**
**   BLUETOOTH VENDOR INTERFACE LIBRARY FUNCTIONS
**
*****************************************************************************/

static int huawei_init(const bt_vendor_callbacks_t* p_cb, unsigned char* local_bdaddr) {
    char chip_type[PROPERTY_VALUE_MAX] = {0};
    const char* lib_name = BCM_LIB_NAME;

    if (property_get("ro.boot.odm.conn.chiptype", chip_type, "") <= 0) {
        property_get("ro.connectivity.chiptype", chip_type, "");
    }

    if (strcmp(chip_type, "hisi") == 0) {
        lib_name = HISI_LIB_NAME;
    }

    int ret = load_vendor_library(lib_name);
    if (ret != 0) {
        ALOGE("Failed to load vendor library");
        return ret;
    }

    return lib_interface->init(p_cb, local_bdaddr);
}

static int huawei_op(bt_vendor_opcode_t opcode, void* param) {
    return lib_interface->op(opcode, param);
}

static void huawei_cleanup(void) {
    lib_interface->cleanup();
    lib_interface = nullptr;
    dlclose(lib_handle);
}

const bt_vendor_interface_t BLUETOOTH_VENDOR_LIB_INTERFACE = {
        .init = huawei_init,
        .op = huawei_op,
        .cleanup = huawei_cleanup,
};
