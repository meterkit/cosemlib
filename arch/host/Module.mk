
# Host target is implemented using the standard C/C++ compiler
# either on Windows or Linux

LOCAL_DIR = $(call my-dir)/

SOURCES += $(addprefix $(LOCAL_DIR), bsp_flash.c)


