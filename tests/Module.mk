
LOCAL_DIR = $(call my-dir)/

# test_aes128gcm.c test_csm_array.c

SOURCES += $(addprefix $(LOCAL_DIR), main.c  test_fs.c)

