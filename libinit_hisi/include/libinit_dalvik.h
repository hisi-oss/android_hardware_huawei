/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string>

typedef struct dalvik_heap_info {
    std::string heapstartsize;
    std::string heapgrowthlimit;
    std::string heapsize;
    std::string heapminfree;
    std::string heapmaxfree;
    std::string heaptargetutilization;
} dalvik_heap_info_t;

void load_dalvik();
