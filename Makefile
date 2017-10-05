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
OUTDIR		:= $(TOPDIR)build/output/


# FIXME: use ARCH to detect OS
DEFINES += -DIS_WIN32 

# Export them to be sure that they are available in sub-makefiles
export PROJECT
export TARGET
export BOARD
export DEFINES
export OPTIM
export ENABLE_DEP
export ARCH
export TOPDIR
export OUTDIR

# *******************************************************************************
# APPLICATION DEFINITIONS
# List of modules and extra libraries needed to generate project targets
# *******************************************************************************
LIB_STM32F4		:= lib/rtos 


LIB_METER				:= lib/system lib/database lib/application lib/crypto lib/ip
LIB_CLIENT				:= lib/crypto lib/serial lib/util lib/hdlc
LIB_BSP					:= arch/host
LIB_TESTS				:= tests
LIB_EXAMPLE_SERVER		:= examples/server
LIB_EXAMPLE_CLIENT		:= examples/client
LIB_GURUX				:= lib/gurux

export LIB_STM32F4
export LIB_METER
export LIB_BSP
export LIB_TESTS
export LIB_EXAMPLE


# *******************************************************************************
# SERVER CONFIGURATION
# *******************************************************************************
ifeq ($(MAKECMDGOALS), server)

APP_MODULES 	:= src $(LIB_METER) $(LIB_BSP) $(LIB_EXAMPLE_SERVER)
APP_LIBPATH 	:= 
APP_LIBS 		:= 

endif

# *******************************************************************************
# CLIENT CONFIGURATION
# *******************************************************************************
ifeq ($(MAKECMDGOALS), client)

APP_MODULES 	:= src $(LIB_CLIENT) $(LIB_EXAMPLE_CLIENT) $(LIB_GURUX)
APP_LIBPATH 	:= 
APP_LIBS 		:= 

endif

# *******************************************************************************
# TESTS CONFIGURATION
# *******************************************************************************
ifeq ($(MAKECMDGOALS), tstu)

APP_MODULES 	:= src $(LIB_METER) $(LIB_BSP) $(LIB_TESTS)
APP_LIBPATH 	:= 
APP_LIBS 		:= 

endif

# *******************************************************************************
# BUILD ENGINE
# *******************************************************************************
include build/Main.mk

server: $(OBJECTS)
	$(call linker, $(OBJECTS), $(APP_LIBS), cosem_server)

client: $(OBJECTS)
	$(call linker, $(OBJECTS), $(APP_LIBS), cosem_client)
	
tstu: $(OBJECTS)
	$(call linker, $(OBJECTS), $(APP_LIBS), cosem_tests)
	
clean:
	@echo "Cleaning generated files..."
	$(VERBOSE) $(RM) -rf $(OUTDIR)/*.o $(OUTDIR)/*.d $(OUTDIR)/*.gcov $(OUTDIR)/*.gcov.htm

wipe:
	@echo "Wiping output directory..."
	$(VERBOSE) $(RM) -rf $(OUTDIR)


# *******************************************************************************
# 								   END OF MAKEFILE								*
# *******************************************************************************
