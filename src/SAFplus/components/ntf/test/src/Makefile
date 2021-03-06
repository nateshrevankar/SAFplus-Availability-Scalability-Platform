# Copyright (C) 2002-2012 OpenClovis Solutions Inc.  All Rights Reserved.
# This file is available  under  a  commercial  license  from  the
# copyright  holder or the GNU General Public License Version 2.0.
#
# The source code for  this program is not published  or otherwise 
# divested of  its trade secrets, irrespective  of  what  has been 
# deposited with the U.S. Copyright office.
# 
# This program is distributed in the  hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied  warranty  of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
# General Public License for more details.
# 
# For more  information,  see the  file COPYING provided with this
# material.
################################################################################
# ModuleName  : gms
# $File: //depot/dev/main/Andromeda/Saryu/SAFplus/components/gms/test/unit-test/client/Makefile $
# $Author: ssahukar $
# $Date: 2006/11/03 $
################################################################################
# Description :
#
# Makefile for tmpcomp component server
#
# This Makefile assumes the following:
#	- CLOVIS_ROOT environment variable is specified properly
#	- Entire source tree under $(CLOVIS_ROOT)/SAFplus is checked out
#
#
################################################################################

# Component name (using all lowercase):

COMPNAME	:= ntf_test

# List local source files needed for the component server:
SRC_FILES   := main.c init_clean_test.c notification_allocate_free_test.c notification_filter_allocate_free.c obj_creat_delete_ptr_val.c notification_send_subscribe.c

# List other SAFplus components of which client libraries your component
# depends on:

DEP_COMP_LIST	:= ioc buffer utils debug gms osal timer rmd eo ntf

#utils osal timer buffer ioc rmd eo amf  event debug
# FIXME: Indirect dependencies via client library needs need to be
# added by hand for now:
DEP_COMP_LIST	+= cnt

# List SAFplus libraries needed to get this server built (or None):

ASP_LIBS	:= libClGms.a \
                   libClOsal.a \
                   libClCnt.a \
                   libClBuffer.a \
                   libClRmd.a \
                   libClTimer.a \
                   libClIoc.a \
                   libClEo.a \
                   libClAmfClient.a \
                   libClLogClient.a \
                   libClUtils.a \
                   libClEventClient.a \
                   libClIdl.a         \
                   libClIdlPtr.a  \
                   libClNameClient.a \
                   libClCorClient.a \
                   libClDebugClient.a \
				   libClMain.a \
				   libClNtf.a \
				   libClPMClient.a \
				   libClAlarmClient.a \
				   libClOmClient.a \
				   libClMsoClient.a \
				   libClAlarmUtils.a \
				   libClTxnAgent.a \
				   libClAlarmXdr.a
                   
# List any third party libs needed to get this server built (or None):

THIRDPARTY_LIBS	:= None

# List any system libraries needed to get this server built (or None)

SYS_LIBS	:= -lpthread -lcheck -lcurses

# Executable name (when building server exectuable):
# Notice the '=' in the assignment.

EXE_NAME	= $(COMPNAME)


include $(CLOVIS_ROOT)/SAFplus/mk/make-server.mk
