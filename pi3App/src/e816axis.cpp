// vi: sw=2 ts=2 sts=2 expandtab smarttab
#include "pi3.h"

#define PI3_E816_MAX_AXES   4
static const char *e816_axes="ABCD";

#define NO_MOVEMENT


E816Axis::E816Axis(PI3Controller *controller, int axis_num)
  :  PI3Axis(controller, axis_num)
{
  assert(axis_num < PI3_E816_MAX_AXES);
  id_ = e816_axes[axis_num];
  queryCalibrationParameters();
}

void E816Axis::queryCalibrationParameters() {
  int param;
  double value;
  asynStatus ret;

  const char *param_names[] = {
    "unknown",
    "VADGain",
    "VADOffset",
    "PADGain",
    "PADOffset",
    "DAGain",
    "DAOffset",
    "Ksen",
    "Osen",
    "Kpzt",
    "Opzt",
    };

  for (param=1; param <= 10; param++) {
    ret = pc_->writeReadDouble(value, "SPA? %c %d", id_, param);
    if (ret == asynSuccess) {
      asynPrint(pasynUser_, ASYN_TRACE_FLOW,
                "%s: axis[%c].%s = %f\n",
                __func__, id_, param_names[param], value);
      printf("axis[%c].%s = %f\n", id_, param_names[param], value);
    }
  }
}

asynStatus E816Axis::setClosedLoop(bool enabled) {
  int servo = enabled ? 1 : 0;
#ifndef NO_MOVEMENT
  return pc_->write("SVO %c %d", id_, servo);
#else
  return asynError;
#endif
}

asynStatus E816Axis::poll(bool *report_moving) {
  this->PI3Axis::poll(report_moving);
  int value;

  asynStatus ret = pc_->writeReadInt(value, "OVF? %c", id_);
  if (ret == asynSuccess) {
    value = value ? 1 : 0;
    setDoubleParam(pc_->motorLowLimit_, value);
    setDoubleParam(pc_->motorHighLimit_, value);
  }

  int axis_moving=0;
  pc_->getIntegerParam(axisNo_, pc_->motorStatusMoving_, &axis_moving);

  if (axis_moving) {
    if (asynSuccess == pc_->writeReadInt(value, "ONT? %c", id_)) {
      // check on target status
      if (value) {
        // if moving and on-target, set motion finished
        finished_moving();
        *report_moving = false;
      }
    }
  }

  return asynSuccess;
}

asynStatus E816Axis::queryPosition() {
  asynStatus ret = pc_->writeReadDouble(encoderPos_, "POS? %c", id_);
  if (ret == asynSuccess) {
    double scaled=encoderPos_ * pc_->getUnitScale();
    setDoubleParam(pc_->motorEncoderPosition_, scaled);
    setDoubleParam(pc_->motorPosition_, scaled);
    callParamCallbacks();
  }

  return asynSuccess;
}

asynStatus E816Axis::stop(double acceleration)
{
  // no way to 'stop' - maybe open the loop?
  finished_moving();
  return asynSuccess;
}

asynStatus E816Axis::moveVelocity(double min_velocity, double max_velocity, double acceleration) {
  asynPrint(pasynUser_, ASYN_TRACE_ERROR|ASYN_TRACE_FLOW,
            "%s: no way to move at a specific velocity\n", __func__);
  return asynError;
}

asynStatus E816Axis::home(double min_velocity, double max_velocity, double acceleration, int forwards) {
  int closed_loop;

  if (asynSuccess != pc_->writeReadInt(closed_loop, "SVO? %c", id_)) {
    return asynError;
  }

  // Open-loop
  pc_->write("SVO %c 0", id_);

  // Set the absolute output voltage to 0V
  pc_->write("SVA %c 0.0", id_);

  // Reset the original closed-loop status
  return pc_->write("SVO %c %d", closed_loop);
}

asynStatus E816Axis::move(double position, int relative, double min_velocity,
                          double max_velocity, double acceleration)
{
  static const char* functionName = "E816Axis::move";
  asynStatus ret = asynError;
  position /= pc_->getUnitScale();

  asynPrint(pc_->getAsynUser(), ASYN_TRACE_FLOW,
            "%s:%s: axis %d: move to %g\n",
            driverName, functionName, id_, position);
  printf("%s:%s: axis %d: move to %g\n",
         driverName, functionName, id_, position);

#ifndef NO_MOVEMENT
  if (relative) {
    ret = pc_->write("MVR %c %f", id_, position);
  } else {
    ret = pc_->write("MOV %c %f", id_, position);
  }
  setIntegerParam(pc_->motorStatusMoving_, true);
  setIntegerParam(pc_->motorStatusDone_, false);
#endif

  return ret;
}

asynStatus E816Axis::setPGain(epicsFloat64 value) {
  printf("setpgain %f\n", value);
  return asynSuccess;
}

asynStatus E816Axis::setIGain(epicsFloat64 value) {
  return asynError;
}

asynStatus E816Axis::setDGain(epicsFloat64 value) {
  return asynError;
}
