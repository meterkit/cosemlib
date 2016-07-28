# *****************************************************************************
# * Copyright Itron SAS.                                                       *
# * This computer program includes confidential proprietary information and    *
# * is a trade secret of Itron SAS. All use, disclosures                       *
# * and/or reproduction is prohibited unless authorized in writing.            *
# * All Rights Reserved                                                        *
# *****************************************************************************/

# *******************************************************************************
# @file		footer.mk
# @brief	Alchemy makefile footer : Contains all targets.
#
# @product	Alchemy
# @compagny	Itron
# @site		Chasseneuil
# *******************************************************************************

# Include all dependencies targets for existing *.o
-include $(DEPENDENCIES) # the dash include means that the file will be included only if it exists, without generating any error

all: $(OBJECTS)

$(addprefix $(OUTPUT_ROOT)/, %.o): %.cpp
	@echo "Building file: $(notdir $@)"
	$(VERBOSE) $(MKDIR) -p "$(dir $@)"
	$(VERBOSE) $(CC) $(CFLAGS) $(DEFINES) $(addprefix -I, $(sort $(INCLUDES))) $(DEPEND_FLAGS) -o $@ $<

$(addprefix $(OUTPUT_ROOT)/, %.o): %.c
	@echo "Building file: $(notdir $@)"
	$(VERBOSE) $(MKDIR) -p "$(dir $@)"
	$(VERBOSE) $(CC) $(CFLAGS) $(DEFINES) $(addprefix -I, $(sort $(INCLUDES))) $(DEPEND_FLAGS) -o $@ $<

$(addprefix $(OUTPUT_ROOT)/, %.o): %.s
	@echo "Building file: $(notdir $@)"
	$(VERBOSE) $(MKDIR) -p "$(dir $@)"
	$(VERBOSE) $(AS) $(ASFLAGS) -o $@ $< 

clean:
	@echo "Cleaning generated files..."
	$(VERBOSE) $(RM) -rf *.o *.d *.gcov *.gcov.htm $(OUTPUT_ROOT)

lint:
	$(VERBOSE) $(LINT) -i$(SRC_ROOT)/build/lint $(LINT_FLAGS) $(LINT_POLICIES) alchemy_policy.lnt $(DEFINES) $(addprefix -I, $(sort $(INCLUDES))) $(CPP_FILES)
	
# *******************************************************************************
# 								   END OF SUB MAKEFILE							*
# *******************************************************************************
