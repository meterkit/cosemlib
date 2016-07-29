# *******************************************************************************
# Main makefile project
# This makefile calls all the modules defined in the config.mk file
# *******************************************************************************

# *******************************************************************************
# DEFAULT DEFINITIONS
# These definitions can be overloaded from the command line
# *******************************************************************************
PROJECT		?= example
TARGET 		?= release
BOARD 		?= host
OPTIM 		?= high
ENABLE_DEP 	?= true
ARCH		?= host-freertos-gcc

TOPDIR		:= ./

# Export them to be sure that they are available in sub-makefiles
export PROJECT
export TARGET
export BOARD
export DEFINES
export OPTIM
export ENABLE_DEP
export ARCH
export TOPDIR

# *******************************************************************************
# APPLICATION DEFINITIONS
# List of modules and extra libraries needed to generate project targets
# *******************************************************************************

LIB_METER		:= lib/system lib/database lib/application lib/crypto lib/rtos lib/transport
LIB_BSP			:= arch/host
LIB_TESTS		:= tests tests/unity
LIB_EXAMPLE		:= examples/server

APP_MODULES 	:= src $(LIB_TESTS) $(LIB_METER) $(LIB_BSP)
APP_LIBPATH 	:= 
APP_LIBS 		:= -lws2_32 -lpsapi -lwinmm
APP_LINK_FILE	:=
APP_EXECUTABLE	:= cosem_tests

# *******************************************************************************
# BUILD ENGINE
# *******************************************************************************
include build/Main.mk

# *******************************************************************************
# PROJECT CUSTOM TARGETS
# *******************************************************************************

PHONY: all
all: $(OBJECTS) link

link:
ifndef COMPONENT
	@echo "Invoking: Linker"
	$(VERBOSE) $(LD) $(APP_LINK_FILE) $(LDFLAGS) $(OBJECTS) $(APP_LIBS)
	@echo "Finished building target: $(EXECUTABLE)"
	@echo " "
endif

clean:
	@echo "Cleaning generated files..."
	$(VERBOSE) $(RM) -rf *.o *.d *.gcov *.gcov.htm $(OUTDIR)

# *******************************************************************************
# 								   END OF MAKEFILE								*
# *******************************************************************************
