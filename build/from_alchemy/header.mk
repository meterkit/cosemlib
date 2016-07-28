# *******************************************************************************
# * Copyright Itron SAS.                                                       	*
# * This computer program includes confidential proprietary information and    	*
# * is a trade secret of Itron SAS. All use, disclosures                       	*
# * and/or reproduction is prohibited unless authorized in writing.            	*
# * All Rights Reserved                                                        	*
# *******************************************************************************

# *******************************************************************************
# @file		header.mk
# @brief	Alchemy makefile header.
#
# This file contains all useful variable definitions and compiler definitions.
#
# @product	Alchemy
# @compagny	Itron
# @site		Chasseneuil
# *******************************************************************************


# *******************************************************************************
# 						SPECIFIC COMPILER DEFINITIONS   						*
# *******************************************************************************

ifeq (gcc, $(findstring gcc, $(ARCH)))
include $(SRC_ROOT)/build/makefiles/gcc/gcc_header.mk
endif

ifeq (keil, $(findstring keil, $(ARCH)))
include $(SRC_ROOT)/build/makefiles/keil/keil_header.mk
endif


# *******************************************************************************
# 							COMPILER DEFINITIONS   								*
# *******************************************************************************
include $(SRC_ROOT)/build/makefiles/env.mk


# *******************************************************************************
# 										TOOLS	  	 							*
# *******************************************************************************
RM			= $(ALCHEMY_MSYS)/rm
MKDIR		= $(ALCHEMY_MSYS)/mkdir


# *******************************************************************************
# 								LINT DEFINITIONS	   							*
# *******************************************************************************

# lint options
LINT_FLAGS    = -zero

# *******************************************************************************
# 						LIST OF SOURCE FILES TO BUILD  							*
# *******************************************************************************


# Remove all the default implicit rules
.SUFFIXES:

# Filter out some extension that need to be compiled differently
CPP_FILES = $(filter %.cpp, $(SOURCES))
C_FILES = $(filter %.c, $(SOURCES))
ASM_FILES = $(filter %.s, $(SOURCES))

CURRENT_COMP := $(notdir $(CURDIR))

# We add the test files
ifeq ($(CURRENT_COMP), $(filter $(CURRENT_COMP), $(TEST_LIST)))
CPP_TEST_FILES = $(filter %.cpp, $(TEST_FILES))
OBJECTS += $(addprefix $(OUTPUT_ROOT)/, $(CPP_TEST_FILES:.cpp=.o))
endif

# We add the source files, automatic STUB files management
ifeq ($(CURRENT_COMP), $(filter $(CURRENT_COMP), $(STUB_LIST)))

CPP_STUB_FILES = $(filter %.cpp, $(STUB_FILES))
OBJECTS += $(addprefix $(OUTPUT_ROOT)/, $(CPP_STUB_FILES:.cpp=.o))

else

#ifeq ($(CURRENT_COMP), $(filter $(CURRENT_COMP), $(COMP_LIST)))

# We add the full path to the list of object so that 'make' can find them (ie, they match) in the footer generic rules
OBJECTS += $(addprefix $(OUTPUT_ROOT)/, $(CPP_FILES:.cpp=.o))
OBJECTS += $(addprefix $(OUTPUT_ROOT)/, $(C_FILES:.c=.o))
OBJECTS += $(addprefix $(OUTPUT_ROOT)/, $(ASM_FILES:.s=.o))
#endif

endif

# *******************************************************************************
# 									INCLUDES 									*
# *******************************************************************************

# Project include
INCLUDES += $(WORKSPACE_ROOT) \
			$(SRC_ROOT)/build/projects/$(PROJECT)

# *******************************************************************************
# 							AUTOMATIC ALCHEMY DEFINITIONS						*
# *******************************************************************************

ifeq ($(TARGET), debug)
	CFLAGS  += -g
    DEFINES += -DLITTLE_ENDIAN -DALCHEMY_DEBUG
ifeq ($(NO_DALCHEMY_TRACE), true)
	#Nothing to do, trace is not added
else
	DEFINES += -DALCHEMY_TRACE
endif    
else
	DEFINES += -DLITTLE_ENDIAN
endif


ifeq ($(OPTIM), none)
	#No optimisation
	CFLAGS += -O0 
else ifeq ($(OPTIM), restricted)
	#restricted optimisation
	CFLAGS += -O1
else ifeq ($(OPTIM), high)
	#high optimisation
	CFLAGS += -O2
else ifeq ($(OPTIM), maximum)
	#maximum optimisation
	CFLAGS += -O3
else ifeq ($(OPTIM), space)
	#space optimisation
	CFLAGS += -Ospace
else ifeq ($(OPTIM), time)
	#time optimisation
	CFLAGS += -Otime
else
	#high optimisation
	CFLAGS += -O2
endif


# *******************************************************************************
# 								   END OF SUB MAKEFILE							*
# *******************************************************************************
