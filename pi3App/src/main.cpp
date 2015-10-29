/*
FILENAME... PI3MotorDriver.cpp
USAGE...


*/

#include "pi3.h"

static struct ELLLIST PI3List;
static int PI3ListInitialized = 0;

#ifndef strnchr

char* strnchr(const char* str, size_t len, char c) {
  if (!str)
    return NULL;

  while (len > 0 && *str != '\0') {
    if (*str == c) {
      return (char*)str;
    }
    str++;
    len--;
  }
  return NULL;
}

#endif

bool addToList(const char *portName, PI3Controller *drv) {
    if (!PI3ListInitialized) {
        PI3ListInitialized = 1;
        ellInit(&PI3List);
    } else if (findByPortName(portName) != NULL) {
        fprintf(stderr, "ERROR: Re-using portName=%s\n", portName);
        return false;
    }

    PI3Node *pNode = (PI3Node*)calloc(1, sizeof(PI3Node));
    pNode->portName = epicsStrDup(portName);
    pNode->pController = drv;
    ellAdd(&PI3List, (ELLNODE*)pNode);
    return true;
}

PI3Controller* findByPortName(const char *portName) {
    PI3Node *pNode;
    static const char *functionName = "findByPortName";

    // Find this
    if (!PI3ListInitialized) {
        printf("%s:%s: ERROR, PI3 list not initialized\n",
            driverName, functionName);
        return NULL;
    }

    pNode = (PI3Node*)ellFirst(&PI3List);
    while(pNode) {
        if (!strcmp(pNode->portName, portName)) {
            return pNode->pController;
        }
        pNode = (PI3Node*)ellNext((ELLNODE*)pNode);
    }

    printf("%s: PI3 on port %s not found\n",
        driverName, portName);
    return NULL;
}

///// E816CreateController
//
/** Creates a new PI3Controller object.
  * Configuration command, called directly or from iocsh
  * \param[in] type              The type of the controller [Use GCS for fully GCS-compatible controllers] (GCS, E-755, ...)
  * \param[in] portName          The name of the asyn port that will be created for this driver
  * \param[in] PI3PortName      The name of the drvAsynIPPPort that was created previously to connect to the PI3 controller
  * \param[in] numAxes           The number of axes that this controller supports
  * \param[in] pollPeriod        The time in ms between polls
  */
extern "C" int E816CreateController(const char *portName, const char *PI3PortName, int numAxes,
                                   int pollPeriod)
{
  new E816Controller(portName, PI3PortName, numAxes, pollPeriod/1000.);
  return(asynSuccess);
}


/** Code for iocsh registration */
static const iocshArg E816CreateControllerArg0 = {"Port name", iocshArgString};
static const iocshArg E816CreateControllerArg1 = {"PI3 port name", iocshArgString};
static const iocshArg E816CreateControllerArg2 = {"Number of axes", iocshArgInt};
static const iocshArg E816CreateControllerArg3 = {"Poll period (ms)", iocshArgInt};
static const iocshArg * const E816CreateControllerArgs[] = {&E816CreateControllerArg0,
                                                               &E816CreateControllerArg1,
                                                               &E816CreateControllerArg2,
                                                               &E816CreateControllerArg3
                                                            };
static const iocshFuncDef E816CreateControllerDef = {"E816CreateController", 4, E816CreateControllerArgs};
static void E816CreateControllerCallFunc(const iocshArgBuf *args)
{
  E816CreateController(args[0].sval, args[1].sval, args[2].ival, args[3].ival);
}

/***********************************************************************/
static void PI3MotorRegister(void)
{
  iocshRegister(&E816CreateControllerDef, E816CreateControllerCallFunc);
}

extern "C" {
epicsExportRegistrar(PI3MotorRegister);
}


