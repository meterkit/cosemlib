#*****************************************************************************
# * Copyright Itron SAS.                                                       *
# * This computer program includes confidential proprietary information and    *
# * is a trade secret of Itron SAS. All use, disclosures                       *
# * and/or reproduction is prohibited unless authorized in writing.            *
# * All Rights Reserved                                                        *
# *****************************************************************************/


# *******************************************************************************
# 									VARIABLES									*
# *******************************************************************************
LIBPATH = $(ARM_LIB)
OUTPUT_FILENAME	= $(PROJECT)
USERLIBPATH = $(OUTPUT_ROOT)
EXECUTABLE ?= $(OUTPUT_FILENAME).axf

# BLA: other way to keep vtable:
# Compiler options: --vfe --no_rtti could also be replaced by -Dvirtual=' __attribute__((section("API_implementation")))
# Linker options: --vfemode=force_no_rtti to be replaced by   --keep="*.o(API_implementation)"

# We are building the LIBRARY list to pass to the linker:
# Libraries are stored in OUTPUT_ROOT/component_name/componenent_name.lib
# From COMPONENTS, we retrieve the component's name: $(notdir $(COMPONENTS))
# For each component of the list, we build the output absolute path
USERLIBPATH += $(foreach COMP, $(notdir $(COMPONENTS)), $(OUTPUT_ROOT)/$(COMP))
LIBRARIES = $(sort $(notdir $(wildcard ${OUTPUT_ROOT}/*/*.o)))

# *******************************************************************************
# 								LINK TARGET AND OPTIONS							*
# *******************************************************************************
LD 		= $(ARM)/armlink
LDFLAGS = --cpu=Cortex-M4 --vfemode=force_no_rtti \
 		  $(addprefix --libpath=, $(LIBPATH)) \
 		  $(addprefix --userlibpath=, $(USERLIBPATH)) \
 		  --entry=Reset_Handler --info=summarysizes --info=unused --info=totals --info=veneers \
 		  --xref --map --symbols --autoat --summary_stderr --callgraph \
 		  --list=$(OUTPUT_FILENAME).map --output $(OUTPUT_ROOT)/$(EXECUTABLE)

link:
ifndef COMPONENT
	@echo "Invoking: Linker"
	@echo $(LIBRARIES) >  $(OUTPUT_ROOT)/list.txt
	$(VERBOSE) $(LD) $(SCATTER) $(LDFLAGS) --via $(OUTPUT_ROOT)/list.txt
	@echo "Finished building target: $(EXECUTABLE)"
	@echo " "
endif

# *******************************************************************************
# 								   END OF MAKEFILE								*
# *******************************************************************************
