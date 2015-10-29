TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += pi3

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install pi3.dbd into <top>/dbd
DBD += pi3.dbd

# specify all source files to be compiled and added to the library
pi3_SRCS += main.cpp pi3controller.cpp pi3axis.cpp e816axis.cpp

mc100_LIBS += motor asyn
mc100_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

