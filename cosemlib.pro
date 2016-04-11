TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CFLAGS_DEBUG      += -O0 -pedantic -std=c99 -ggdb -Wall
QMAKE_CFLAGS_RELEASE    += -Os -pedantic -std=c99 -Wall

win32 {
    LIBS +=  libws2_32 -lpsapi
    DEFINES += CONFIG_NATIVE_WINDOWS
}


VPATH += src
VPATH += tests
VPATH += unity
VPATH += examples/server
VPATH += examples/crypto
VPATH += sys

INCLUDEPATH += src
INCLUDEPATH += tests
INCLUDEPATH += unity
INCLUDEPATH += examples/server
INCLUDEPATH += examples/crypto
INCLUDEPATH += sys

OTHER_FILES += README.md

# ====================================================
# Cosem library
# ====================================================
SOURCES += csm_array.c csm_ber.c csm_channel.c csm_association.c csm_services.c \
    csm_axdr_codec.c csm_security.c

HEADERS += csm_array.h csm_config.h csm_ber.h csm_channel.h csm_association.h csm_definitions.h csm_services.h \
    csm_axdr_codec.h csm_security.h server_config.h

# ====================================================
# Implementation example: cosem database and transport
# ====================================================
SOURCES += tcp_server.c db_cosem_clock.c db_cosem_associations.c

HEADERS += tcp_server.h db_cosem_clock.h db_cosem_object_list.h db_cosem_associations.h

# ====================================================
# Implementation example: operating system
# ====================================================
SOURCES += clock.c os.c system.c bitfield.c

HEADERS += clock.h os.h bitfield.h

# ====================================================
# Implementation example: meter application
# ====================================================

SOURCES += app_calendar.c app_database.c

HEADERS += app_calendar.h app_database.h

# ====================================================
# Implementation example: crypto algorithms
# ====================================================

SOURCES += gcm.c aes.c cipher.c cipher_wrap.c

HEADERS += aes.h cipher.h cipher_internal.h config.h gcm.h

# ====================================================
# Test files
# ====================================================
SOURCES += test_csm_array.c test_aes128gcm.c

# ====================================================
# Main entry point of the test project
# ====================================================
SOURCES += main.c

# ====================================================
# Unity test framework
# ====================================================

SOURCES += unity.c unity_fixture.c
HEADERS += unity.h \
           unity_internals.h \
           unity_fixture.h \
           unity_fixture_internals.h \
           unity_fixture_malloc_overrides.h

