
LOCAL_DIR = $(call my-dir)/

SOURCES += $(addprefix $(LOCAL_DIR), event_groups.c list.c queue.c tasks.c timers.c)
INCLUDES += $(addprefix $(LOCAL_DIR), include)
