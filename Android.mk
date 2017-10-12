LOCAL_PATH:= $(call my-dir)

SDK_VERSION = $(shell if [ $(PLATFORM_SDK_VERSION) -ge 21 ]; then echo 23; else echo 19; fi)
SHARED_LIBRARIES_PATH := libs/$(SDK_VERSION)/$(TARGET_CPU_ABI)

$(shell cp $(LOCAL_PATH)/etc/openvoice_profile.json $(TARGET_OUT_ETC))
$(shell cp $(LOCAL_PATH)/etc/blacksiren.json $(TARGET_OUT_ETC))
$(shell cp -r $(LOCAL_PATH)/workdir_cn $(TARGET_OUT)/workdir_cn)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		$(call all-java-files-under, src) \
		src/com/rokid/openvoice/IVoiceService.aidl \
		src/com/rokid/openvoice/IVoiceCallback.aidl

LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true
#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_PACKAGE_NAME := openvoice_process
include $(BUILD_PACKAGE)
##########################################################################
#
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
		libopenvoice2:$(SHARED_LIBRARIES_PATH)/libopenvoice2.so \
		libbsiren:$(SHARED_LIBRARIES_PATH)/libbsiren.so \
		libopenvoice:$(SHARED_LIBRARIES_PATH)/libopenvoice.so \
		libpoco:$(SHARED_LIBRARIES_PATH)/libpoco.so \
		libprotobuf-rokid-cpp-full:$(SHARED_LIBRARIES_PATH)/libprotobuf-rokid-cpp-full.so \
		libr2mvdrbf:$(SHARED_LIBRARIES_PATH)/libr2mvdrbf.so \
		libr2ssp:$(SHARED_LIBRARIES_PATH)/libr2ssp.so \
		libr2vt:$(SHARED_LIBRARIES_PATH)/libr2vt.so \
		libspeech:$(SHARED_LIBRARIES_PATH)/libspeech.so \
		libztvad:$(SHARED_LIBRARIES_PATH)/libztvad.so \
#		libcrypto:$(SHARED_LIBRARIES_PATH)/libcrypto.so \
		libssl:$(SHARED_LIBRARIES_PATH)/libssl.so \
		libtinyalsa:$(SHARED_LIBRARIES_PATH)/libtinyalsa.so \
		libz:$(SHARED_LIBRARIES_PATH)/libz.so

include $(BUILD_MULTI_PREBUILT)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
		$(call all-java-files-under, jar) \
		src/com/rokid/openvoice/IVoiceService.aidl \
		src/com/rokid/openvoice/IVoiceCallback.aidl

LOCAL_JACK_ENABLED = disabled

LOCAL_MODULE := rokid_openvoice_sdk

include $(BUILD_STATIC_JAVA_LIBRARY)
