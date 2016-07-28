#*****************************************************************************
# * Copyright Itron SAS.                                                       *
# * This computer program includes confidential proprietary information and    *
# * is a trade secret of Itron SAS. All use, disclosures                       *
# * and/or reproduction is prohibited unless authorized in writing.            *
# * All Rights Reserved                                                        *
# *****************************************************************************/


# *******************************************************************************
# 					OPERATING SYSTEM ENVIRONMENT VARIABLES						*
# *******************************************************************************

ALCHEMY_ARM        ?= C:/Keil/ARM/ARMCC/bin
ALCHEMY_MINGW      ?= C:/mingw/bin
ALCHEMY_MSYS       ?= C:/msys/1.0/bin
ALCHEMY_MAKE	   ?= C:/mingw/bin/mingw32-bin
ALCHEMY_LINT       ?= C:/lint/lint-nt.exe
ALCHEMY_PERL       ?= C:/Perl/bin/perl.exe
ALCHEMY_DOXYGEN    ?= C:/Progra~1/doxygen/bin/doxygen.exe

ALCHEMY_ARM_INC    ?= 	C:/Keil/ARM/RV31/INC \
	      	      		C:/Keil/ARM/ARMCC/include \
		      			C:/Keil/ARM/ARMCC/bin

ALCHEMY_ARM_LIB    ?= 	C:/Keil/ARM/RV31/LIB \
		      			C:/Keil/ARM/ARMCC/lib

# This global variable is used for make to build N files in parallel and speed-up the compilation time
NUMBER_OF_JOBS     ?= 4

# Verbosity shows all the commands that are executed by the makefile, and their arguments
VERBOSE            ?= @


# *******************************************************************************
# 									MAKEFILE VARIABLES							*
# *******************************************************************************
ARM         = $(ALCHEMY_ARM)
MAKE		= $(ALCHEMY_MAKE)
MINGW       = $(ALCHEMY_MINGW)
RM			= $(ALCHEMY_MSYS)/rm
MKDIR		= $(ALCHEMY_MSYS)/mkdir
MSYS        = $(ALCHEMY_MSYS)
LINT        = $(ALCHEMY_LINT)
PERL        = $(ALCHEMY_PERL)
DOXYGEN     = $(ALCHEMY_DOXYGEN)
ARM_INC     = $(ALCHEMY_ARM_INC)
ARM_LIB     = $(ALCHEMY_ARM_LIB)


# *******************************************************************************
# 								   END OF MAKEFILE								*
# *******************************************************************************
