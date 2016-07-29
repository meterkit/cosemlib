# *******************************************************************************
# Main.mk
# Build engine targetted for component/module architecture.
# TODO:
#   - Export GCC configuration into external file
#   - Support more toolchains
# *******************************************************************************

# this turns off the suffix rules built into make
.SUFFIXES:

OUTDIR			:= $(TOPDIR)build/output/

ifeq (gcc, $(findstring gcc, $(ARCH)))
# Compiler setting
CC      = gcc
AR      = ar
AS      = as
LD 		= gcc
LDFLAGS = -Wl,-subsystem,console -o $(OUTDIR)$(APP_EXECUTABLE) $(addprefix -L, $(APP_LIBPATH))

# FIXME: different CFLAGS for debug/release targets
DEFINES	+= -DUNICODE -DCONFIG_NATIVE_WINDOWS
CFLAGS  = -c -pipe -fno-keep-inline-dllexport -g -O0 -pedantic -std=c99 -ggdb -Wall -Wextra


ifeq ($(ENABLE_DEP), true)
	# List of dependencies
	DEPENDENCIES = $(OBJECTS:%.o=%.d)
	# Dependency flags
	DEPEND_FLAGS = -MMD
endif

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

# Figure out where we are. Taken from Android build system thanks!
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


SOURCES 	:=
INCLUDES 	:=

# Include all the modules sub-makefiles in one command
-include $(patsubst %, %/Module.mk, $(ALL_MODULES))

# Deduct objects to build 
OBJECTS := $(addprefix $(OUTDIR),$(patsubst %.c, %.o, $(filter %.c,$(SOURCES))))

# Include generated dependency files, if any
-include $(DEPENDENCIES)


INCLUDES += $(ALL_MODULES)

$(addprefix $(OUTDIR), %.o): %.c
	@echo "Building file: $(notdir $@)"
	$(VERBOSE) $(MKDIR) -p "$(dir $@)"
	$(VERBOSE) $(CC) $(CFLAGS) $(DEFINES) $(addprefix -I, $(INCLUDES)) $(DEPEND_FLAGS) -o $@ $<

$(addprefix $(OUTDIR), %.o): %.s
	@echo "Building file: $(notdir $@)"
	$(VERBOSE) $(MKDIR) -p "$(dir $@)"
	$(VERBOSE) $(AS) $(ASFLAGS) -o $@ $< 

# *******************************************************************************
# GENERIC TARGETS
# *******************************************************************************

PHONY: all
all: $(OBJECTS)
ifndef MODULE
	@echo "Invoking: Linker"
	$(VERBOSE) $(LD) $(APP_LINK_FILE) $(LDFLAGS) $(OBJECTS) $(APP_LIBS)
	@echo "Finished building target: $(APP_EXECUTABLE)"
	@echo " "
endif

clean:
	@echo "Cleaning generated files..."
	$(VERBOSE) $(RM) -rf *.o *.d *.gcov *.gcov.htm

wipe:
	@echo "Wiping output directory..."
	$(VERBOSE) $(RM) -rf $(OUTDIR)

# *******************************************************************************
# 								   END OF MAKEFILE								*
# *******************************************************************************
