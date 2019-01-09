LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

SDL_TFF_PATH := ../SDL_ttf

SDL_IMAGE_PATH := ../SDK_image

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/$(SDL_TTF_PATH) $(LOCAL_PATH)/$(SDL_IMAGE_PATH)

LOCAL_SRC_FILES := controllerlogic.c drawlogic.c gamelogic.c main.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_image

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
