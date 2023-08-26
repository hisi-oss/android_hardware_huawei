#
# Copyright (C) 2022 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

LOCAL_PATH := $(call my-dir)

#
# Global Flags
#
HW_MKBOOTIMG := $(TARGET_KERNEL_SOURCE)/tools/mkbootimg

#
# kernel.img
#

INTERNAL_CUSTOM_BOOTIMAGE_ARGS := --base $(BOARD_KERNEL_BASE) --pagesize $(BOARD_KERNEL_PAGESIZE) --kernel $(INSTALLED_KERNEL_TARGET) --cmdline "$(BOARD_KERNEL_CMDLINE) buildvariant=userdebug"

$(INSTALLED_BOOTIMAGE_TARGET): $(HW_MKBOOTIMG) $(AVBTOOL) $(INTERNAL_BOOTIMAGE_FILES) $(BOOTIMAGE_EXTRA_DEPS) $(INSTALLED_KERNEL_TARGET)
	$(call pretty,"Target boot image: $@")
	$(hide) $(HW_MKBOOTIMG) $(INTERNAL_CUSTOM_BOOTIMAGE_ARGS) $(INTERNAL_MKBOOTIMG_VERSION_ARGS) $(BOARD_MKBOOTIMG_ARGS) --output $@
	$(hide) $(call assert-max-image-size,$@,$(BOARD_BOOTIMAGE_PARTITION_SIZE))
	$(hide) $(AVBTOOL) add_hash_footer \
	  --image $@ \
	  --partition_size $(BOARD_BOOTIMAGE_PARTITION_SIZE) \
	  --partition_name boot $(INTERNAL_AVB_BOOT_SIGNING_ARGS) \
	  $(BOARD_AVB_BOOT_ADD_HASH_FOOTER_ARGS)

#
# ramdisk.img
#

INSTALLED_BOOTRAMDISKIMAGE_TARGET := $(PRODUCT_OUT)/boot_ramdisk.img
INTERNAL_CUSTOM_BOOTRAMDISKIMAGE_ARGS := --kernel /dev/null --ramdisk $(BUILT_RAMDISK_TARGET) --cmdline "buildvariant=$(TARGET_BUILD_VARIANT)"

.PHONY: bootramdiskimage
bootramdiskimage: $(INSTALLED_BOOTRAMDISKIMAGE_TARGET)

$(INSTALLED_BOOTRAMDISKIMAGE_TARGET): $(MKBOOTIMG) $(AVBTOOL) $(BUILT_RAMDISK_TARGET)
	$(call pretty,"Target bootramdisk image: $@")
	$(hide) $(MKBOOTIMG) $(INTERNAL_CUSTOM_BOOTRAMDISKIMAGE_ARGS) $(INTERNAL_MKBOOTIMG_VERSION_ARGS) --output $@
	$(hide) $(call assert-max-image-size,$@,$(BOARD_BOOTRAMDISKIMAGE_PARTITION_SIZE))
	$(hide) $(AVBTOOL) add_hash_footer \
	  --image $@ \
	  --partition_size $(BOARD_BOOTRAMDISKIMAGE_PARTITION_SIZE) \
	  --partition_name boot $(INTERNAL_AVB_BOOT_SIGNING_ARGS) \
	  $(BOARD_AVB_BOOT_ADD_HASH_FOOTER_ARGS)

INSTALLED_RADIOIMAGE_TARGET += $(INSTALLED_BOOTRAMDISKIMAGE_TARGET)

#
# recovery_ramdisk.img
#

INTERNAL_CUSTOM_RECOVERYIMAGE_ARGS := --kernel /dev/null --ramdisk $(recovery_ramdisk) --cmdline "buildvariant=$(TARGET_BUILD_VARIANT)"

$(INSTALLED_RECOVERYIMAGE_TARGET): $(MKBOOTIMG) $(AVBTOOL) $(recovery_ramdisk)
	@echo "----- Making recovery image ------"
	$(hide) $(MKBOOTIMG) $(INTERNAL_CUSTOM_RECOVERYIMAGE_ARGS) $(INTERNAL_MKBOOTIMG_VERSION_ARGS) --output $@
	$(hide) $(call assert-max-image-size,$@,$(BOARD_RECOVERYIMAGE_PARTITION_SIZE))
	$(hide) $(AVBTOOL) add_hash_footer \
	  --image $@ \
	  --partition_size $(BOARD_RECOVERYIMAGE_PARTITION_SIZE) \
	  --partition_name recovery $(INTERNAL_AVB_RECOVERY_SIGNING_ARGS) \
	  $(BOARD_AVB_RECOVERY_ADD_HASH_FOOTER_ARGS)

#
# ramdisk-recovery_vendor.img
#

INSTALLED_RECVENDOR_RAMDISK_TARGET := $(PRODUCT_OUT)/ramdisk-recovery_vendor.img
INSTALLED_RECVENDOR_RAMDISK_PLACEHOLDER_TARGET := $(PRODUCT_OUT)/ramdisk-recovery_vendor

$(INSTALLED_RECVENDOR_RAMDISK_PLACEHOLDER_TARGET):
	$(hide) mkdir -p $@/vendor
	$(hide) touch $@/vendor/placeholder

$(INSTALLED_RECVENDOR_RAMDISK_TARGET): $(MKBOOTFS) $(MINIGZIP) $(INSTALLED_RECVENDOR_RAMDISK_PLACEHOLDER_TARGET)
	$(call pretty,"Target recovery_vendor ramdisk image: $@")
	$(hide) $(MKBOOTFS) -d $(PRODUCT_OUT) $(INSTALLED_RECVENDOR_RAMDISK_PLACEHOLDER_TARGET) | $(MINIGZIP) > $@

#
# recovery_vendor.img
#

INSTALLED_RECVENDORIMAGE_TARGET := $(PRODUCT_OUT)/recovery_vendor.img
INTERNAL_CUSTOM_RECVENDORIMAGE_ARGS := --kernel /dev/null --ramdisk $(INSTALLED_RECVENDOR_RAMDISK_TARGET) --cmdline "buildvariant=$(TARGET_BUILD_VARIANT)"

.PHONY: recoveryvendorimage
recoveryvendorimage: $(INSTALLED_RECVENDORIMAGE_TARGET)

$(INSTALLED_RECVENDORIMAGE_TARGET): $(MKBOOTIMG) $(AVBTOOL) $(INSTALLED_RECVENDOR_RAMDISK_TARGET)
	$(call pretty,"Target recovery_vendor image: $@")
	$(hide) $(MKBOOTIMG) $(INTERNAL_CUSTOM_RECVENDORIMAGE_ARGS) $(INTERNAL_MKBOOTIMG_VERSION_ARGS) --output $@
	$(hide) $(call assert-max-image-size,$@,$(call get-hash-image-max-size,$(BOARD_RECVENDORIMAGE_PARTITION_SIZE)))
	$(hide) $(AVBTOOL) add_hash_footer \
	  --image $@ \
	  --partition_size $(BOARD_RECVENDORIMAGE_PARTITION_SIZE) \
	  --partition_name recovery $(INTERNAL_AVB_RECOVERY_SIGNING_ARGS) \
	  $(BOARD_AVB_RECOVERY_ADD_HASH_FOOTER_ARGS)

INSTALLED_RADIOIMAGE_TARGET += $(INSTALLED_RECVENDORIMAGE_TARGET)
