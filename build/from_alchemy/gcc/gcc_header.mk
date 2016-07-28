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
CC      = $(ALCHEMY_MINGW)/g++
AR      = $(ALCHEMY_MINGW)/ar
AS      = $(ALCHEMY_MINGW)/as

CFLAGS = -c -std=c++11 -Wno-write-strings -ffunction-sections -fdata-sections -fpermissive

# Alchemy and Keil lint policies
LINT_POLICIES += 


ifeq ($(ENABLE_DEP), true)
	# List of dependencies
	DEPENDENCIES = $(OBJECTS:%.o=%.d)
	# Dependency flags
#	DEPEND_FLAGS = --md --no_depend_system_headers --phony_targets --depend=$(@:%.o=%.d)
endif


# *******************************************************************************
# 								   END OF MAKEFILE								*
# *******************************************************************************
