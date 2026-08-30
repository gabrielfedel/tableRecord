# SPDX-FileCopyrightText: 1997 Argonne National Laboratory
#
# SPDX-License-Identifier: EPICS

# Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG

# Directories to build, any order
DIRS += configure

DIRS += tableRecordApp
tableRecordApp_DEPEND_DIRS = configure

DIRS += test
test_DEPEND_DIRS = tableRecordApp

ifneq (YES,$(TABLERECORD_SKIP_EXAMPLE))
DIRS += exampleApp
exampleApp_DEPEND_DIRS = tableRecordApp
endif

DIRS += iocBoot
iocBoot_DEPEND_DIRS = tableRecordApp

include $(TOP)/configure/RULES_TOP
