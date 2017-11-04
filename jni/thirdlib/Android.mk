SDK_BLACKSIREN := blacksiren/libbsiren/libs/armeabi-v7a
DEP_LIBRARIES  := thirdlib/$(ANDROID_VERSION)

include $(CLEAR_VARS)
LOCAL_MODULE := libbsiren
LOCAL_SRC_FILES := $(SDK_BLACKSIREN)/libbsiren.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libr2mvdrbf
LOCAL_SRC_FILES := $(SDK_BLACKSIREN)/libr2mvdrbf.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libr2ssp
LOCAL_SRC_FILES := $(SDK_BLACKSIREN)/libr2ssp.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libztvad
LOCAL_SRC_FILES := $(SDK_BLACKSIREN)/libztvad.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libr2vt
LOCAL_SRC_FILES := $(SDK_BLACKSIREN)/libr2vt.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libtinyalsa
LOCAL_SRC_FILES := $(DEP_LIBRARIES)/libtinyalsa.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libssl
LOCAL_SRC_FILES := thirdlib/$(MY_TARGET_PLATFORM_LEVEL)/libssl.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libcrypto
LOCAL_SRC_FILES := thirdlib/$(MY_TARGET_PLATFORM_LEVEL)/libcrypto.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libz
LOCAL_SRC_FILES := $(DEP_LIBRARIES)/libz.so
include $(PREBUILT_SHARED_LIBRARY)
