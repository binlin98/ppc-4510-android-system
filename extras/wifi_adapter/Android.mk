LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := wifi_adapter.c

LOCAL_FORCE_STATIC_EXECUTABLE:=true
LOCAL_STATIC_LIBRARIES += libc libcutils liblog

LOCAL_CFLAGS := -std=c99
LOCAL_MODULE := wifi_adapter

include $(BUILD_EXECUTABLE)
