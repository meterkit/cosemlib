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

# Compiler setting
CC      = $(ALCHEMY_ARM)/armcc
AR      = $(ALCHEMY_ARM)/armar
AS      = $(ALCHEMY_ARM)/armasm

CFLAGS  = --cpu=Cortex-M4 --split_sections --signed_chars --min_array_alignment=4 --diag_warning=228 --vfe -c --diag_style=gnu
ASFLAGS = --cpu=Cortex-M4

# Alchemy and Keil lint policies
LINT_POLICIES += $(SRC_ROOT)/build/makefiles/keil/lib-keil.lnt

# Standard RL-RTX includes
INCLUDES += $(ALCHEMY_ARM_INC)


ifeq ($(ENABLE_DEP), true)
	# List of dependencies
	DEPENDENCIES = $(OBJECTS:%.o=%.d)
	# Dependency flags
	DEPEND_FLAGS = --md --no_depend_system_headers --phony_targets --depend=$(@:%.o=%.d)
endif

ifeq ($(FULL_CPP), true)
	# Enable exception and rtti
	CFLAGS  += --exceptions --rtti --rtti_data
else
	# Disable exception and rtti
	CFLAGS  += --no_rtti --no_exceptions
endif


# *******************************************************************************
# 								   END OF MAKEFILE								*
# *******************************************************************************
