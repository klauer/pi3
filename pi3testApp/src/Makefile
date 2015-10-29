TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = pi3test
# pi3test.dbd will be created and installed
DBD += pi3test.dbd

# pi3test.dbd will be made up from these files:
pi3test_DBD += base.dbd

# Include dbd files from all support applications:
pi3test_DBD += drvAsynIPPort.dbd motorRecord.dbd motorSupport.dbd asyn.dbd pi3.dbd asSupport.dbd

# Add all the support libraries needed by this IOC
pi3test_LIBS += asyn pi3 motor autosave

# pi3test_registerRecordDeviceDriver.cpp derives from pi3test.dbd
pi3test_SRCS += pi3test_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
pi3test_SRCS_DEFAULT += pi3testMain.cpp
pi3test_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#pi3test_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
pi3test_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

