LOCAL_PATH := prebuilts/vndk

include $(CLEAR_VARS)
LOCAL_MODULE := libprotobuf-cpp-full-v29
LOCAL_MULTILIB := both
LOCAL_SRC_FILES_arm := v29/arm/arch-arm-armv7-a-neon/shared/vndk-core/libprotobuf-cpp-full.so
LOCAL_SRC_FILES_arm64 := v29/arm64/arch-arm64-armv8-a/shared/vndk-core/libprotobuf-cpp-full.so
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_TARGET_ARCH := arm arm64
LOCAL_MODULE_TAGS := optional
LOCAL_CHECK_ELF_FILES := false
LOCAL_VENDOR_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libprotobuf-cpp-lite-v29
LOCAL_MULTILIB := both
LOCAL_SRC_FILES_arm := v29/arm/arch-arm-armv7-a-neon/shared/vndk-core/libprotobuf-cpp-lite.so
LOCAL_SRC_FILES_arm64 := v29/arm64/arch-arm64-armv8-a/shared/vndk-core/libprotobuf-cpp-lite.so
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_TARGET_ARCH := arm arm64
LOCAL_MODULE_TAGS := optional
LOCAL_CHECK_ELF_FILES := false
LOCAL_VENDOR_MODULE := true
include $(BUILD_PREBUILT)
