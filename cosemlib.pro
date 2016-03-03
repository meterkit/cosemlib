TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CFLAGS_DEBUG      += -O0 -pedantic -std=c99 -ggdb -Wall
QMAKE_CFLAGS_RELEASE    += -Os -pedantic -std=c99 -Wall

win32 {
    LIBS +=  libws2_32 -lpsapi
}


VPATH += src
VPATH += tests
VPATH += unity
VPATH += examples/server

INCLUDEPATH += src
INCLUDEPATH += tests
INCLUDEPATH += unity
INCLUDEPATH += examples/server

OTHER_FILES += README.md

# ====================================================
# Cosem library
# ====================================================
SOURCES += csm_array.c csm_ber.c csm_channel.c csm_association.c csm_services.c

HEADERS += csm_array.h csm_config.h csm_ber.h csm_channel.h csm_association.h csm_definitions.h csm_services.h


# ====================================================
# Implementation example: cosem database and transport
# ====================================================
SOURCES += tcp_server.c csm_database.c csm_db_gen.h

HEADERS += tcp_server.h csm_database.h csm_data_id_gen.h

# ====================================================
# Implementation example: operating system
# ====================================================
SOURCES += clock.c

HEADERS += clock.h

# ====================================================
# Implementation example: meter application
# ====================================================

SOURCES += calendar.c

HEADERS += calendar.h

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


