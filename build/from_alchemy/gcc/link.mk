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
OUTPUT_FILENAME	= $(PROJECT)
USERLIBPATH 	= $(OUTPUT_ROOT)
EXECUTABLE 		= $(OUTPUT_FILENAME).exe


# With GCC, the following command is used to link against a library
# -l xyz
# The linker searches a standard list of directories for the library, which is actually a file named libxyz.a.

#automatic linking
# LIBRARIES += $(foreach COMP, $(notdir $(COMPONENTS)), -l$(COMP))
USERLIBPATH += $(foreach COMP, $(notdir $(COMPONENTS)), $(OUTPUT_ROOT)/$(COMP))

PRE_LINK_CMD = 
POST_LINK_CMD =

# *******************************************************************************
# 								LINK TARGET AND OPTIONS							*
# *******************************************************************************
LD 		= $(MINGW)/g++
LDFLAGS = -o $(OUTPUT_ROOT)/$(EXECUTABLE) $(addprefix -L, $(USERLIBPATH))


# *******************************************************************************
# 								   END OF MAKEFILE								*
# *******************************************************************************
