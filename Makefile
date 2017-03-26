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
LIB_STM32F4		:= lib/rtos 


LIB_METER		:= lib/system lib/database lib/application lib/crypto lib/transport
LIB_BSP			:= arch/host
LIB_TESTS		:= tests tests/embunit
LIB_EXAMPLE		:= examples/server

# *******************************************************************************
# TEST EXECUTABLE
# *******************************************************************************
# APP_MODULES 	:= src $(LIB_TESTS) $(LIB_METER) $(LIB_BSP)
APP_LIBPATH 	:= 
APP_LIBS 		:= 
APP_LINK_FILE	:=
# APP_EXECUTABLE	:= cosem_tests
APP_EXECUTABLE	:= cosem_server

# *******************************************************************************
# SERVER EXAMPLE
# *******************************************************************************
APP_MODULES 	:= src $(LIB_METER) $(LIB_BSP) $(LIB_EXAMPLE)
APP_LIBPATH 	:= 
APP_LIBS 		:= 
APP_LINK_FILE	:=
APP_EXECUTABLE	:= cosem_server

# *******************************************************************************
# BUILD ENGINE
# *******************************************************************************
include build/Main.mk


# *******************************************************************************
# 								   END OF MAKEFILE								*
# *******************************************************************************
