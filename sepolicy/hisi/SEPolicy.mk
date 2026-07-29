#
# SPDX-FileCopyrightText: The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

BOARD_VENDOR_SEPOLICY_DIRS += \
    hardware/huawei/sepolicy/hisi/vendor

SYSTEM_EXT_PRIVATE_SEPOLICY_DIRS += \
    hardware/huawei/sepolicy/hisi/private

SYSTEM_EXT_PUBLIC_SEPOLICY_DIRS += \
    hardware/huawei/sepolicy/hisi/public

# tmp
BOARD_KERNEL_CMDLINE += androidboot.selinux=permissive
SELINUX_IGNORE_NEVERALLOWS := true
