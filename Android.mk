LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
        $(call all-java-files-under, src) 

LOCAL_SHARED_LIBRARIES := \
		libbinder \
		libhardware \
		libbsiren \
		libutils \
		libgpr \
		liblog \
		libgrpc \
		libgrpc++ \
		libprotobuf-rokid-cpp-lite \
		libprotobuf-rokid-cpp-full \
		libspeech_common \
		libspeech \
		librokid_speech_jni

LOCAL_JAVA_LIBRARIES := \
    	framework \
		services 

LOCAL_STATIC_JAVA_LIBRARIES := \
		rokid_speech

LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform
#LOCAL_PRIVILEGED_MODULE := true
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_PACKAGE_NAME := rokid-openvoice-speech-app

include $(BUILD_PACKAGE)
