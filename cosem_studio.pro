
# Specific OS stuff
win32 {
    RC_FILE = icon.rc
    # Let's make everything's static so that we don't need any DLL
    QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static -lpthread
}

QMAKE_CXXFLAGS += -std=c++11

VPATH += src
VPATH += lua
VPATH += tools/cosem_studio

INCLUDEPATH += lua
INCLUDEPATH += src
INCLUDEPATH += tools/cosem_studio

QT += widgets

HEADERS += highlighter.h \
        mainwindow.h \
        i_script.h \
        ThreadQueue.h \
        lua_wrapper.h \
        main.h

SOURCES += highlighter.cpp \
            mainwindow.cpp \
            main.cpp \
            lua_wrapper.cpp


# ----------------------
# LUA
# ----------------------
SOURCES +=  lapi.c \
            lauxlib.c \
            lbaselib.c \
            lbitlib.c \
            lcode.c \
            lcorolib.c \
            lctype.c \
            ldblib.c \
            ldebug.c \
            ldo.c \
            ldump.c \
            lfunc.c \
            lgc.c \
            linit.c \
            liolib.c \
            llex.c \
            lmathlib.c \
            lmem.c \
            loadlib.c \
            lobject.c \
            lopcodes.c \
            loslib.c \
            lparser.c \
            lstate.c \
            lstring.c \
            lstrlib.c \
            ltable.c \
            ltablib.c \
            ltm.c \
            lundump.c \
            lutf8lib.c \
            lvm.c \
            lzio.c

RESOURCES += shaddam.qrc
