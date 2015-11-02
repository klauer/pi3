#include "pi3.h"

/** Creates a new PI3Axis object.
  * \param[in] controller         The PI3 controller
  * \param[in] axis_num           The axis number (1-based)
  */
PI3Axis::PI3Axis(PI3Controller *controller, int axis_num)
  :  asynMotorAxis((asynMotorController*)controller, axis_num)
{
    id_ = axis_num;
    pc_ = controller;
    encoderPos_ = 0.0;
    finished_moving();
}

asynStatus PI3Axis::setClosedLoop(bool enabled) {
    return asynError;
}

asynStatus PI3Axis::finished_moving() {
  setIntegerParam(pc_->motorStatusMoving_, false);
  setIntegerParam(pc_->motorStatusDone_, true);
  return callParamCallbacks();
}

asynStatus PI3Axis::poll(bool *moving) {
  queryPosition();

  /*
    *moving = false;
    finished_moving();
  */

  return asynSuccess;
}

asynStatus PI3Axis::queryPosition() {
  return asynError;
}

asynStatus PI3Axis::stop(double acceleration)
{
  return asynError;
}

asynStatus PI3Axis::moveVelocity(double min_velocity, double max_velocity, double acceleration) {
  return asynError;
}

asynStatus PI3Axis::home(double min_velocity, double max_velocity, double acceleration, int forwards) {
  return asynError;
}

asynStatus PI3Axis::move(double position, int relative, double min_velocity, double max_velocity, double acceleration)
{
  return asynError;
  /*
  static const char* functionName = "PI3Axis::move";
  asynStatus ret;

  position /= pc_->unitScale_;

  asynPrint(pc_->pasynUser_, ASYN_TRACE_FLOW,
    "%s:%s: axis %d: move to %g\n",
    driverName, functionName, id_, position);
  printf("%s:%s: axis %d: move to %g\n",
    driverName, functionName, id_, position);

  ret=pc_->write("set,%d,%.3f", id_, position);
  pc_->check_error();
  return ret;*/
}

asynStatus PI3Axis::setPGain(epicsFloat64 value) {
  return asynError;
}

asynStatus PI3Axis::setIGain(epicsFloat64 value) {
  return asynError;
}

asynStatus PI3Axis::setDGain(epicsFloat64 value) {
  return asynError;
}

asynStatus PI3Axis::isClosedLoop(bool &closed) {
  return asynError;
}
