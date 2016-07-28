# *******************************************************************************
# * Copyright Itron SAS.                                                       	*
# * This computer program includes confidential proprietary information and    	*
# * is a trade secret of Itron SAS. All use, disclosures                       	*
# * and/or reproduction is prohibited unless authorized in writing.            	*
# * All Rights Reserved                                                        	*
# *******************************************************************************

# *******************************************************************************
# @file		link.mk
# @brief	Alchemy makefile linker.
#
# This file contains all useful variable definitions and linker definitions.
#
# @product	Alchemy
# @compagny	Itron
# @site		Chasseneuil
# *******************************************************************************

ifeq (gcc, $(findstring gcc, $(ARCH)))
include $(SRC_ROOT)/build/makefiles/gcc/link.mk
endif

ifeq (keil, $(findstring keil, $(ARCH)))
include $(SRC_ROOT)/build/makefiles/keil/link.mk
endif

# *******************************************************************************
# 								   END OF SUB MAKEFILE							*
# *******************************************************************************
