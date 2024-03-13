#define LOG_TAG "libxcollie"

#include <log/log.h>
#include <stdint.h>
#include <stdio.h>

unsigned long __xcollie_start_ex(const char* name, int size, void (*arg3)() __unused,
                                 const char* arg4, unsigned int arg5) {
    ALOGV("%s: name: %s, size: %d, arg4: %s, arg5: 0x%x", __func__, name, size,
          arg4 ? arg4 : "NULL", arg5);
    return 0;
}

void xcollie_end(unsigned long handle) {
    ALOGV("%s: process_handle: %lu", __func__, handle);
}

void xcollie_init() {
    ALOGV("%s: called", __func__);
}
