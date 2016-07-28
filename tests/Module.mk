
MODULE = $(call my-dir)/

SOURCES += $(addprefix $(MODULE), main.c test_aes128gcm.c test_association.c test_csm_array.c test_fs.c)

