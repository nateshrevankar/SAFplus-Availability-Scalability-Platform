################################################################################
# 
# This Makefile builds the software in the defined subdirectories.
# It expects that the SAFplus build environment is defined
# by "sourcing" clasp.env in your model's build/local directory.
#
# This Makefile is auto-generated by OpenClovis IDE
#
################################################################################

# Component name (using all lowercase):

EXENAME := dummyComp

# List local source files needed for the component server:

SRC_FILES := $(wildcard *.c)
CPP_FILES := $(wildcard *.C) $(wildcard *.cxx)

# List ASP libraries needed to get this server built (or None):
ASP_LIBS := libmw.a

EXTRA_CPPFLAGS :=

# Executable name (when building server exectuable):
# Notice the '=' in the assignment.
EXE_NAME = $(EXENAME)


CPPFLAGS += -I$(ASP_CONFIG) 

BUILD_CPLUSPLUS = 1

EXTRA_LDLIBS +=

# ---BEGIN_APPLICATION_CODE---
# Note if you make changes only within these existing APPLICATION_CODE blocks,
# then your changes will be preserved even if you make modifications in the
# IDE and regenerate the source.

# Important variables:
# OBJ_DIR  - Where all the object files are generated
# LIB_DIR  - Where all the libs are generated
# BIN_DIR  - Where the final product is generated

# To add libraries to the link line and to the dependencies append them to 
# EXTRA_LDLIBS.  For example, let's make a library named "libmystuff.a":
# EXTRA_LDLIBS    += $(LIB_DIR)/libmystuff.a

# To recursively make in dependent subdirectories, append them to SUB_DIRS
# SUB_DIRS += 

# ---END_APPLICATION_CODE---
 
include $(CLOVIS_ROOT)/ASP/mk/make-server.mk

# ---BEGIN_APPLICATION_CODE---
# Define additional targets BELOW, so that they are not used as the default 
# target.

# To make specific additional targets (other then C or c++ files in this
# directory -- these are automatically added), add them here.  For example,
# to add a script that needs to be copied:
# all: $(BIN_DIR)/myScript

# Add the rules to build the additional targets here:
# For example:
# $(BIN_DIR)/myScript: myScript.sh
#       $(MKDIR) $(BIN_DIR)
#       $(CP) $< $@

# Above I had an example of an external library "libmystuff.a".  Now I must 
# define the target to build it.  Let us imagine that the source for this 
# library exists in a subdirectory called "common" off of the parent.

# $(LIB_DIR)/libmystuff.a: ../common/mystuff.c
#   $(MAKE) -C ../common

# Note that the OpenClovis defined make variables will still be defined in the
# invocation of this sub-makefile.  They must be used to ensure that binaries
# and libraries are placed in the correct directories, and that the correct
# compiler is used (the compiler changes when doing cross-builds).

# For more information (and an example Makefile) see the OpenClovis SDK user 
# guide.

# ---END_APPLICATION_CODE---
