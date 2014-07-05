LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

GL_APP_PATH := ../../../GLApp
GL_ENGINE_PATH := ../..

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/$(GL_ENGINE_PATH)/include \
	$(LOCAL_PATH)/$(GL_ENGINE_PATH)/libs/include \
	$(LOCAL_PATH)/$(GL_ENGINE_PATH)/libs/include-shared \
	$(LOCAL_PATH)/$(GL_APP_PATH)/src

LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/../SDL2-2.0.3/src/main/android/SDL_android_main.c \
	$(wildcard $(LOCAL_PATH)/$(GL_ENGINE_PATH)/src/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_ENGINE_PATH)/src/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_ENGINE_PATH)/src/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_ENGINE_PATH)/src/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_ENGINE_PATH)/src/*/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_ENGINE_PATH)/src/*/*/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_ENGINE_PATH)/src/*/*/*/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_ENGINE_PATH)/src/*/*/*/*/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_ENGINE_PATH)/src/*/*/*/*/*/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_ENGINE_PATH)/src/*/*/*/*/*/*/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_APP_PATH)/src/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_APP_PATH)/src/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_APP_PATH)/src/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_APP_PATH)/src/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_APP_PATH)/src/*/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_APP_PATH)/src/*/*/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_APP_PATH)/src/*/*/*/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_APP_PATH)/src/*/*/*/*/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_APP_PATH)/src/*/*/*/*/*/*/*/*/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(GL_APP_PATH)/src/*/*/*/*/*/*/*/*/*/*.cpp) \

LOCAL_MODULE := main
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_LDLIBS := -lGLESv2 -lGLESv3 -llog
APP_CPPFLAGS += -std=c++11

include $(BUILD_SHARED_LIBRARY)