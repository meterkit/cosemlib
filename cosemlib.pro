TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CFLAGS_DEBUG      += -O0 -pedantic -std=c99 -ggdb -Wall
QMAKE_CFLAGS_RELEASE    += -Os -pedantic -std=c99 -Wall

VPATH += src
VPATH += tests
VPATH += unity

INCLUDEPATH += src
INCLUDEPATH += tests
INCLUDEPATH += unity

OTHER_FILES += README.md

# ====================================================
# Cosem library
# ====================================================
SOURCES += csm_array.c csm_ber.c

HEADERS += csm_array.h csm_config.h csm_ber.h

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


