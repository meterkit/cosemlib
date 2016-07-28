
MODULE = $(call my-dir)/

SOURCES += $(addprefix $(MODULE),aes.c cipher.c cipher_wrap.c gcm.c)

