# *******************************************************************************
# Main.mk
# Build engine entry point, specific arguments must be passed through Makefile
# variables.
# Mandatory arguments:
#   - Module full path
#
# *******************************************************************************

# this turns off the suffix rules built into make
.SUFFIXES:

OUTDIR			:= $(TOPDIR)output/

ifeq (gcc, $(findstring gcc, $(ARCH)))
# Compiler setting
CC      = $(MINGW)/gcc
AR      = $(MINGW)/ar
AS      = $(MINGW)/as
LD 		= $(CC)
LDFLAGS = -o $(OUTDIR)$(APP_EXECUTABLE) $(addprefix -L, $(USERLIBPATH))

# FIXME: different CFLAGS for debug/release targets
DEFINES	+= -DUNICODE -DCONFIG_NATIVE_WINDOWS
CFLAGS  = -c -pipe -fno-keep-inline-dllexport -g -O0 -pedantic -std=c99 -ggdb -Wall -Wextra

#CC_FLAGS += -MMD
#-include $(OBJFILES:.o=.d)

endif

DEL_FILE      := rm -f
CHK_DIR_EXISTS := test -d
MKDIR         := mkdir -p
COPY_FILE     := cp -f
COPY_DIR      := cp -f -R
MOVE          := mv -f

# Verbosity shows all the commands that are executed by the makefile, and their arguments
VERBOSE            ?= @

# If we want to build one module, overrire the module list
ifdef MODULE
ALL_MODULES = $(MODULE)
else
ALL_MODULES = $(sort $(APP_MODULES))
endif


###########################################################
## Retrieve the directory of the current makefile
## Must be called before including any other makefile!!
###########################################################

# Figure out where we are.
define my-dir
$(strip \
  $(eval LOCAL_MODULE_MAKEFILE := $$(lastword $$(MAKEFILE_LIST))) \
  $(if $(filter $(BUILD_SYSTEM)/% $(OUTDIR)/%,$(LOCAL_MODULE_MAKEFILE)), \
    $(error my-dir must be called before including any other makefile.) \
   , \
    $(patsubst %/,%,$(dir $(LOCAL_MODULE_MAKEFILE))) \
   ) \
 )
endef

###########################################################
## Retrieve the directory of the current makefile
## Must be called before including any other makefile!!
###########################################################

SOURCES 	:=
INCLUDES 	:=

-include $(patsubst %, %/Module.mk, $(ALL_MODULES))

OBJECTS := $(addprefix $(OUTDIR),$(patsubst %.c, %.o, $(filter %.c,$(SOURCES))))

vpath %.c $(sort $(dir $(OBJECTS)))

INCLUDES += $(ALL_MODULES)
$(info Include files $(OBJECTS))

$(addprefix $(OUTDIR), %.o): %.c
	@echo "Building file: $(notdir $@)"
	$(VERBOSE) $(MKDIR) -p "$(dir $@)"
	$(VERBOSE) $(CC) $(CFLAGS) $(DEFINES) $(addprefix -I, $(INCLUDES)) $(DEPEND_FLAGS) -o $@ $<

$(addprefix $(OUTDIR), %.o): %.s
	@echo "Building file: $(notdir $@)"
	$(VERBOSE) $(MKDIR) -p "$(dir $@)"
	$(VERBOSE) $(AS) $(ASFLAGS) -o $@ $< 


# *******************************************************************************
# 								   END OF MAKEFILE								*
# *******************************************************************************
