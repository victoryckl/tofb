LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:= libsavebmp

LOCAL_SRC_FILES:= \
	myfb.c \
	readbmp.c \
	rgb_convert.c \
	savebmp.c \
	_log.c \
	screenshot.c \
	tofb.c
	
LOCAL_STATIC_LIBRARIES := libcutils libc

LOCAL_LDLIBS := -llog

include $(BUILD_STATIC_LIBRARY)

################################################
include $(CLEAR_VARS)

LOCAL_MODULE:= tofb

LOCAL_SRC_FILES:=main.c

LOCAL_STATIC_LIBRARIES := libsavebmp libcutils libc

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)
