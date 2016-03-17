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
    csm_axdr_codec.c

HEADERS += csm_array.h csm_config.h csm_ber.h csm_channel.h csm_association.h csm_definitions.h csm_services.h \
    csm_axdr_codec.h csm_system.h

# ====================================================
# Implementation example: cosem database and transport
# ====================================================
SOURCES += tcp_server.c csm_database.c csm_clock_objects.c

HEADERS += tcp_server.h csm_database.h csm_clock_objects.h csm_db_gen.h

# ====================================================
# Implementation example: operating system
# ====================================================
SOURCES += clock.c os.c system.c bitfield.c

HEADERS += clock.h os.h bitfield.h

# ====================================================
# Implementation example: meter application
# ====================================================

SOURCES += calendar.c

HEADERS += calendar.h

# ====================================================
# Implementation example: crypto algorithms
# ====================================================

SOURCES += aes-gcm.c aes-internal.c aes-internal-dec.c aes-internal-enc.c aes-unwrap.c aes-wrap.c

HEADERS += aes.h aes_i.h aes_wrap.h

# ====================================================
# Test files
# ====================================================
SOURCES += test_csm_array.c

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

