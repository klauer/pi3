#ifndef _PI3_H
#define _PI3_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>

#include <iocsh.h>
#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <epicsExport.h>

#include <asynOctetSyncIO.h>
#include <asynMotorController.h>
#include <asynMotorAxis.h>

#include "pi3axis.h"
#include "pi3controller.h"

#ifndef strnchr
    char* strnchr(const char* str, size_t len, char c);
#endif

#endif
