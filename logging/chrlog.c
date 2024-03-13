#define LOG_TAG "libchrlog"

#include <log/log.h>
#include <stdint.h>
#include <stdio.h>

typedef enum chr_LogPriority {
    CHR_LOG_DEBUG = 0,
    CHR_LOG_INFO,
    CHR_LOG_WARN,
    CHR_LOG_ERROR,
} logPriority;

typedef enum chr_LogTag {
    CHR_LOG_TAG_PLAT = 0,
    CHR_LOG_TAG_WIFI,
    CHR_LOG_TAG_GNSS,
    CHR_LOG_TAG_BT,
    CHR_LOG_TAG_FM,
    CHR_LOG_TAG_NFC,
} logTag;

int32_t __chr_exception(uint32_t errno) {
    ALOGV("%s: errno: %u", __func__, errno);
    return 0;
}

int32_t __chr_exception_para(uint32_t errno, uint8_t* ptr, uint16_t len) {
    ALOGV("%s: errno: %u, ptr: %p, len: %u", __func__, errno, ptr, len);
    return 0;
}

int __chr_printLog(logPriority prio, logTag tag, const char* fmt, ...) {
    ALOGV("%s: prio: %d, tag: %u, fmt: %s", __func__, prio, tag, fmt);
    return 0;
}

int __chr_print_log(logPriority prio, logTag tag, const char* fmt, ...)
        __attribute__((alias("__chr_printLog")));
