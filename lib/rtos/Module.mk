
MODULE = $(call my-dir)/

SOURCES += $(addprefix $(MODULE), event_groups.c list.c queue.c tasks.c timers.c)
INCLUDES += $(addprefix $(MODULE), include)
