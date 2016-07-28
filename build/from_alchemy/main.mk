# ******************************************************************************
# * Copyright Itron SAS.                                                       *
# * This computer program includes confidential proprietary information and    *
# * is a trade secret of Itron SAS. All use, disclosures                       *
# * and/or reproduction is prohibited unless authorized in writing.            *
# * All Rights Reserved                                                        *
# ******************************************************************************

# --== Instructions ==--
#
# This file is the main alchemy makefile targeted to build a single executable
# To run properly, a set of make variables must be passed through the command 
# line. A lot of optional commands are available to fine-tune the build process.
# Please refer to the Alchemy Wiki to learn more.
# 
# In this way, variables are considered as envirnonment variables and therefore
# accessible in sub-makefiles.
#
# It is required that
#
# Example: make -f ${CWD}/../../makefiles/main.mk PROJECT=lr_program ALCHEMY_IMPL=master
#
# Mandatory variables are:
#
# PROJECT			= name of your project, should be located inside the "sub_project" directory
# ALCHEMY_IMPL		= master proxy stub

# *******************************************************************************
# 								   VARIABLES DECLARATION    					*
# *******************************************************************************

# This main makefile must be called within the workspace directory
WORKSPACE_ROOT  = $(subst \,/,$(CURDIR))
SRC_ROOT		= $(abspath $(WORKSPACE_ROOT)/../../..)
PROJECT_ROOT	= $(abspath $(WORKSPACE_ROOT)/../../projects/$(PROJECT))
OUTPUT_ROOT		= $(WORKSPACE_ROOT)/_output/$(PROJECT)/$(TARGET)

# Export variables created in this makefile to the Make environment for the sub-makefiles 
export WORKSPACE_ROOT
export SRC_ROOT

# We retrieve the make goal to setup internal variables
ifeq ($(MAKECMDGOALS), all)
	ACTION := Building
endif
ifeq ($(MAKECMDGOALS), clean)
	ACTION := Cleaning
endif
ifeq ($(MAKECMDGOALS), lint)
	ACTION := Linting
endif

# Default architecture is Keil for ARM Cortex-M3
export ARCH ?= cm3-rlrtx-keil

# Include environment variables
include $(SRC_ROOT)/build/makefiles/env.mk

# Include compiler linker definition
include $(SRC_ROOT)/build/makefiles/link.mk

# Include project specific rules
include $(PROJECT_ROOT)/rules.mk
export TEST_LIST
export STUB_LIST
export COMP_LIST

ifdef COMPONENT
COMPONENTS = $(COMPONENT)
else
# Automatically add the project directory as a component
COMPONENTS += $(PROJECT_ROOT)
endif

# Add the optional components
# To do so, we use several tricks:
#  1) usage of an early evaluation of the variable thanks to :=
#  2) usage of a wildcard % so we can only define the component name in the list (ie, without the full path)
COMPONENTS += $(filter $(addprefix %, $(COMP_LIST)), $(OPTIONAL_COMPONENTS))


# *******************************************************************************
# 								GENERIC BUILD TARGETS							*
# *******************************************************************************
.PHONY: $(COMPONENTS)
$(COMPONENTS):
	@echo "--------------------------------"
	@echo "--- $(ACTION) $(notdir $@)"
	@echo "--------------------------------"
	$(VERBOSE) $(MAKE) -C $@ -j $(NUMBER_OF_JOBS) -f component.mk -I $(SRC_ROOT)/build/makefiles OUTPUT_ROOT=$(OUTPUT_ROOT)/$(notdir $@) $(MAKECMDGOALS)
	@echo " "

PHONY: all
all: $(COMPONENTS) link

.PHONY: clean
clean: $(COMPONENTS)

.PHONY: wipe
wipe:
	$(VERBOSE) $(RM) -rf $(OUTPUT_ROOT)

.PHONY: lint
lint: $(COMPONENTS)


# *******************************************************************************
# 								   END OF MAKEFILE								*
# *******************************************************************************
	
