# *******************************************************************************
# rules.mk
# Definition of generic targets
# *******************************************************************************

# Include all dependencies targets for existing *.o
# -include $(DEPENDENCIES) # the dash include means that the file will be included only if it exists, without generating any error

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

clean:
	@echo "Cleaning generated files..."
	#$(VERBOSE) $(RM) -rf *.o *.d *.gcov *.gcov.htm $(OUT_DIR)


# *******************************************************************************
# 								   END OF SUB MAKEFILE							*
# *******************************************************************************
