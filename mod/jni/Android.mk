LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(LOCAL_PATH)/cachecleaner.cpp  \
  $(wildcard $(LOCAL_PATH)/../thirdparty/yaml-cpp/src/*.cpp) \
  $(wildcard $(LOCAL_PATH)/../thirdparty/yaml-cpp/src/contrib/*.cpp)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../ \
  $(LOCAL_PATH)/../thirdparty/yaml-cpp/include/ \
  $(LOCAL_PATH)/../thirdparty/CuStringMatcher/source \
  $(LOCAL_PATH)/../thirdparty/CuLogger/source

LOCAL_MODULE := CacheCleaner

LOCAL_MODULE_FILENAME := CacheCleaner

COMMAND := -static -Wl,--strip-all -O3 -fPIC -pipe -g0 -flto=thin -Wl,--gc-sections -ffunction-sections -fdata-sections -falign-functions -falign-loops -Wl,--sort-section=alignment -fvisibility=hidden -ffast-math -fno-stack-protector -fno-builtin -fmerge-all-constants -fmerge-constants -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-strict-aliasing -fno-rtti -fno-exceptions -fomit-frame-pointer -fno-stack-check -fms-extensions -fdeclspec -fexceptions

LOCAL_CFLAGS := $(COMMAND)
LOCAL_CPPFLAGS := -std=c++17
LOCAL_LDFLAGS := $(COMMAND)

include $(BUILD_EXECUTABLE)