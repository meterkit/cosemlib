
ifeq (gcc, $(findstring gcc, $(ARCH)))
# Compiler setting
CC      = $(ALCHEMY_MINGW)/g++
AR      = $(ALCHEMY_MINGW)/ar
AS      = $(ALCHEMY_MINGW)/as

CFLAGS = -c -std=c++11 -Wno-write-strings -ffunction-sections -fdata-sections -fpermissive
endif

DEL_FILE      := rm -f
CHK_DIR_EXISTS := test -d
MKDIR         := mkdir -p
COPY_FILE     := cp -f
COPY_DIR      := cp -f -R
MOVE          := mv -f

# Verbosity shows all the commands that are executed by the makefile, and their arguments
VERBOSE            ?= @

###########################################################
## Retrieve the directory of the current makefile
## Must be called before including any other makefile!!
###########################################################

SOURCES = $(wildcard *.c) $(wildcard $(ALL_MODULES)/*.c)

# Filter out some extension that need to be compiled differently
CPP_FILES	= $(filter %.cpp, $(SOURCES))
C_FILES		= $(filter %.c, $(SOURCES))
ASM_FILES 	= $(filter %.s, $(SOURCES))


# # We add the full path to the list of object so that 'make' can find them (ie, they match) in the footer generic rules
OBJECTS = $(addprefix $(OUT_DIR)/, $(CPP_FILES:.cpp=.o))
OBJECTS += $(addprefix $(OUT_DIR)/, $(C_FILES:.c=.o))
OBJECTS += $(addprefix $(OUT_DIR)/, $(ASM_FILES:.s=.o))


$(addprefix $(OUT_DIR)/, %.o): %.cpp
	@echo "Building file: $(notdir $@)"
	$(VERBOSE) $(MKDIR) -p "$(dir $@)"
	$(VERBOSE) $(CC) $(CFLAGS) $(DEFINES) $(addprefix -I, $(sort $(INCLUDES))) $(DEPEND_FLAGS) -o $@ $<

$(addprefix $(OUT_DIR)/, %.o): %.c
	@echo "Building file: $(notdir $@)"
	$(VERBOSE) $(MKDIR) -p "$(dir $@)"
	$(VERBOSE) $(CC) $(CFLAGS) $(DEFINES) $(addprefix -I, $(sort $(INCLUDES))) $(DEPEND_FLAGS) -o $@ $<

$(addprefix $(OUT_DIR)/, %.o): %.s
	@echo "Building file: $(notdir $@)"
	$(VERBOSE) $(MKDIR) -p "$(dir $@)"
	$(VERBOSE) $(AS) $(ASFLAGS) -o $@ $< 



