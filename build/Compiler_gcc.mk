
# Compiler setting
CC      = $(ALCHEMY_MINGW)/gcc
AR      = $(ALCHEMY_MINGW)/ar
AS      = $(ALCHEMY_MINGW)/as

CFLAGS = -c -Wno-write-strings -ffunction-sections -fdata-sections -fpermissive


ifeq ($(ENABLE_DEP), true)
	# List of dependencies
	DEPENDENCIES = $(OBJECTS:%.o=%.d)
	# Dependency flags
#	DEPEND_FLAGS = --md --no_depend_system_headers --phony_targets --depend=$(@:%.o=%.d)
endif


# *******************************************************************************
# 									VARIABLES									*
# *******************************************************************************
OUTPUT_FILENAME	= $(PROJECT)
EXECUTABLE 		= $(OUTPUT_FILENAME).exe

# *******************************************************************************
# 								LINK TARGET AND OPTIONS							*
# *******************************************************************************
LD 		= $(MINGW)/g++
LDFLAGS = -o $(OUTPUT_ROOT)/$(EXECUTABLE) $(addprefix -L, $(USERLIBPATH))

