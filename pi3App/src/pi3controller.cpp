// vi: sw=2 ts=2 sts=2 expandtab smarttab
#include "pi3.h"

E816Controller::E816Controller(const char *portName, const char *PI3PortName,
                               int numAxes, double pollPeriod)
    : PI3Controller(portName, PI3PortName, numAxes, pollPeriod, "\n", "\n") {
  int axis;
  E816Axis *pAxis;

  // Create the axis objects
  for (axis=0; axis<numAxes; axis++) {
    pAxis = new E816Axis(this, axis);
    // has PID settings
    pAxis->setIntegerParam(motorStatusGainSupport_, 1);
    pAxis->setIntegerParam(motorStatusHasEncoder_, 1);
  }
  startPoller(pollPeriod/1000., pollPeriod/1000., 2);

}

/** Creates a new PI3Controller object.
  * \param[in] portName          The name of the asyn port that will be created for this driver
  * \param[in] PI3PortName     The name of the drvAsynIPPPort that was created previously to connect to the PI3 controller
  * \param[in] numAxes           The number of axes that this controller supports
  * \param[in] movingPollPeriod  The time between polls when any axis is moving
  * \param[in] idlePollPeriod    The time between polls when no axis is moving
  */
PI3Controller::PI3Controller(const char *portName, const char *PI3PortName, int numAxes,
                             double pollPeriod, const char *input_eos, const char *output_eos)
  :  asynMotorController(portName, numAxes, NUM_PI3_PARAMS,
                         asynInt32Mask | asynFloat64Mask | asynUInt32DigitalMask,
                         asynInt32Mask | asynFloat64Mask | asynUInt32DigitalMask,
                         ASYN_CANBLOCK | ASYN_MULTIDEVICE,
                         1, // autoconnect
                         0, 0)  // Default priority and stack size
{
  asynStatus status;
  static const char *functionName = "PI3Controller::PI3Controller";

  unitScale_ = PI3_UNIT_SCALE;
  if (!addToList(portName, this)) {
    printf("%s:%s: Init failed", driverName, portName);
    return;
  }

  movingPollPeriod_ = pollPeriod;
  idlePollPeriod_ = pollPeriod;
  printf("Poll period: %f\n", pollPeriod);
  createParam(PI3_STATUS_STRING, asynParamOctet, &PI3StatusString_);

  setStringParam(PI3StatusString_, "Startup");
  timeout_ = PI3_TIMEOUT;

  /* Connect to PI3 controller */
  status = pasynOctetSyncIO->connect(PI3PortName, 0, &pasynUser_, NULL);
  if (status) {
    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
      "%s:%s: cannot connect to PI3 controller\n",
      driverName, functionName);
  }

  status = pasynOctetSyncIO->setInputEos(pasynUser_, input_eos, 1);
  if (status) {
    asynPrint(pasynUser_, ASYN_TRACE_ERROR|ASYN_TRACE_FLOW,
      "%s: Unable to set input EOS on %s: %s\n",
      functionName, PI3PortName, pasynUser_->errorMessage);
  }

  status = pasynOctetSyncIO->setOutputEos(pasynUser_, output_eos, 1);
  if (status) {
    asynPrint(pasynUser_, ASYN_TRACE_ERROR|ASYN_TRACE_FLOW,
      "%s: Unable to set output EOS on %s: %s\n",
      functionName, PI3PortName, pasynUser_->errorMessage);
  }

  queryVersion();

}

asynStatus PI3Controller::queryVersion() {
  size_t num_read=0;
  char ver_string[PI3_STRING_SIZE];
  static const char* functionName = "PI3Controller::queryVersion";
  asynStatus ret = writeRead(ver_string, &num_read, "*IDN?");
  if (num_read > 0 && ret == asynSuccess) {
    printf("%s: controller version: %s\n", functionName, ver_string);
  }
  return asynSuccess;
}

asynStatus PI3Controller::write(const char *fmt, va_list argptr) {
  size_t nwrite;
  asynStatus status;
  const char *functionName="write";
  const int buf_size = PI3_STRING_SIZE;
  char buf[buf_size];

  vsnprintf(buf, buf_size, fmt, argptr);

  lock();

#if DEBUG
  fprintf(stderr, "%s:%s: %s\n", driverName, functionName, buf);
#endif

  asynPrint(pasynUser_, ASYN_TRACE_FLOW,
    "%s:%s: %s\n",
    driverName, functionName, buf);

  status = pasynOctetSyncIO->write(pasynUser_,
                                   buf, strlen(buf),
                                   timeout_, &nwrite);
  unlock();

  return status;
}

asynStatus PI3Controller::write(const char *fmt, ...) {
  va_list argptr;
  va_start(argptr,fmt);
  asynStatus ret=write(fmt, argptr);
  va_end(argptr);
  return ret;
}

asynStatus PI3Controller::writeReadDouble(double &value, const char *fmt, va_list argptr) {
  char input[PI3_STRING_SIZE];
  size_t nread;
  size_t nwrite;
  asynStatus status;
  int eomReason;
  const char *functionName="writeReadDouble";
  const int buf_size = PI3_STRING_SIZE;
  char buf[buf_size];

  vsnprintf(buf, buf_size, fmt, argptr);

  strncpy(outString_, buf, buf_size);
  lock();

  asynPrint(pasynUser_, ASYN_TRACEIO_DRIVER,
    "%s:%s: Write: %s\n",
    driverName, functionName, buf);

  status = pasynOctetSyncIO->writeRead(pasynUser_,
                                       buf, strlen(buf),
                                       input, PI3_STRING_SIZE,
                                       timeout_, &nwrite, &nread, &eomReason);

  asynPrint(pasynUser_, ASYN_TRACEIO_DRIVER,
            "%s:%s: Read (%lub): %s\n",
            driverName, functionName, nread, input);

  unlock();

  if (status == asynSuccess && nread > 0) {
      value = atof(input);
  }

  return status;
}
asynStatus PI3Controller::writeRead(char *input, size_t* nread, const char *fmt, va_list argptr) {

  size_t nwrite;
  asynStatus status;
  int eomReason;
  const char *functionName="writeRead";
  const int buf_size = PI3_STRING_SIZE;
  char buf[buf_size];

  vsnprintf(buf, buf_size, fmt, argptr);

  strncpy(outString_, buf, buf_size);
  lock();
#if DEBUG
  fprintf(stderr, "%s:%s: write: %s\n",
    driverName, functionName, buf);
#endif

  asynPrint(pasynUser_, ASYN_TRACEIO_DRIVER,
    "%s:%s: Write: %s\n",
    driverName, functionName, buf);

  status = pasynOctetSyncIO->writeRead(pasynUser_,
                                       buf, strlen(buf),
                                       input, PI3_STRING_SIZE,
                                       timeout_, &nwrite, nread, &eomReason);

#if DEBUG
  fprintf(stderr, "%s:%s: Read (%db): %s\n",
          driverName, functionName, *nread, input);
#endif
  asynPrint(pasynUser_, ASYN_TRACEIO_DRIVER,
            "%s:%s: Read (%lub): %s\n",
            driverName, functionName, *nread, input);

  unlock();

  return status;
}

asynStatus PI3Controller::writeReadInt(int &value, const char *fmt, ...) {
  va_list argptr;
  double dbl_value;

  va_start(argptr,fmt);
  asynStatus ret=writeReadDouble(dbl_value, fmt, argptr);
  va_end(argptr);

  if (ret == asynSuccess) {
    value = (int)dbl_value;
  }
  return ret;
}

asynStatus PI3Controller::writeReadDouble(double &value, const char *fmt, ...) {
  va_list argptr;
  va_start(argptr,fmt);
  asynStatus ret=writeReadDouble(value, fmt, argptr);
  va_end(argptr);
  return ret;
}

asynStatus PI3Controller::writeRead(char *input, size_t* nread, const char *fmt, ...) {
  va_list argptr;
  va_start(argptr,fmt);
  asynStatus ret=writeRead(input, nread, fmt, argptr);
  va_end(argptr);
  return ret;
}

asynStatus
PI3Controller::checkError() {
  int errno;
  if (asynSuccess == writeReadInt(errno, "ERR?")) {
    asynPrint(pasynUser_, ASYN_TRACE_ERROR|ASYN_TRACE_FLOW,
              "%s: error code: %d\n",
              __func__, errno);
  }
  return asynSuccess;
}
