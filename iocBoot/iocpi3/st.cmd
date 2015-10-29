#!../../bin/linux-x86_64/pi3test

< envPaths

## Register all support components
dbLoadDatabase("../../dbd/pi3test.dbd",0,0)
pi3test_registerRecordDeviceDriver(pdbbase) 

epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "NO")
epicsEnvSet("EPICS_CA_ADDR_LIST", "10.3.0.255")

epicsEnvSet("P" "PI3:")
epicsEnvSet("R" "")
epicsEnvSet("PI3_PORT" "PI3")
epicsEnvSet("AS_PREFIX", "$(P)$(R)")

drvAsynIPPortConfigure("IP1" ,"10.3.2.52:4011",0,0,0)

#PI3CreateController(portName, PI3PortName, numAxes, pollPeriod)
# PI3CreateController("$(PI3_PORT)", "IP1", 3, 50)
E816CreateController("$(PI3_PORT)", "IP1", 3, 50)

# asynSetTraceMask("$(PI3_PORT)", 0, 9)
# asynSetTraceMask("IP1", -1, 9)
#
# asynSetTraceIOMask("$(PI3_PORT)", -1, 255)
# asynSetTraceIOMask("IP1", -1, 255)

dbLoadRecords("$(TOP)/db/pi3.db", "P=$(P),R=$(R),PORT=$(PI3_PORT),ADDR=0,TIMEOUT=0.1")
dbLoadTemplate("pi3.sub")

cd $(TOP)/iocBoot/$(IOC)
# < save_restore.cmd
iocInit()

create_monitor_set("auto_positions.req", 5, "P=$(AS_PREFIX)")
create_monitor_set("auto_settings.req", 30, "P=$(AS_PREFIX)")
