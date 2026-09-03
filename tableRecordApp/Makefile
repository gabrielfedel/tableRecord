TOP = ..
include $(TOP)/configure/CONFIG

USR_CPPFLAGS += -DUSE_TYPED_RSET -DUSE_TYPED_DSET -DDUSE_TYPED_DRVET

#DBDINC += tableBase
DBDINC += tableRecord
DBDINC += tablecalcRecord

DBD += tableBase.dbd
DBD += tableSupport.dbd
#DBD += tablecalcSupport.dbd

LIBRARY_IOC += tableRecord
LIBRARY_IOC += tablecalcRecord

tableRecord_SRCS += tableRecord.cpp
tableRecord_SRCS += tableVStr.c
tableRecord_SRCS += tableRecordUtil.cpp
tableRecord_SRCS += devTableSoft.cpp
tableRecord_SRCS += devTablecalcSoft.cpp
tableRecord_SRCS += tableSource.cpp
tableRecord_SRCS += tableSourceRegistrar.cpp
tableRecord_SRCS += tablecalcRecord.cpp

tableRecord_LIBS += pvxsIoc pvxs $(EPICS_BASE_IOC_LIBS)

USR_CPPFLAGS += -DBUILDING_tableRecord_API
API_HEADER = tableRecordAPI.h
INC += tableRecordUtil.h
INC += tableVStr.h

# Reference documentation, generated from tableRecord.dbd.pod
HTMLS_DIR = .
DOCS  += tableRecord.md
HTMLS += tableRecord.html
#DOCS  += tableBase.md
#HTMLS += tableBase.html

include $(TOP)/configure/RULES

$(INSTALL_HTML)/style.css: $(EPICS_BASE)/html/style.css
	$(ECHO) "Installing html $@"
	@$(INSTALL) -d -m $(INSTALL_PERMISSIONS) $< $(@D)

buildInstall: $(INSTALL_HTML)/style.css

ifdef T_A
ifeq (,$(INSTALL_API_HEADERS))
$(error Requires epics-base >= 7.0.4)
endif
endif
