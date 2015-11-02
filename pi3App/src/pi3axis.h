// vi: sw=2 ts=2 sts=2 expandtab
//
#ifndef _PI3_AXIS_H
#define _PI3_AXIS_H


class PI3Controller;


class PI3Axis : public asynMotorAxis
{
public:
  /* These are the methods we override from the base class */
  PI3Axis(PI3Controller *pC, int axis);
  virtual asynStatus move(double position, int relative, double min_velocity, double max_velocity, double acceleration);
  virtual asynStatus moveVelocity(double min_velocity, double max_velocity, double acceleration);
  virtual asynStatus home(double min_velocity, double max_velocity, double acceleration, int forwards);
  virtual asynStatus stop(double acceleration);
  virtual asynStatus poll(bool *moving);
  //asynStatus setPosition(double position);

  /* And these are specific to this class: */
  virtual asynStatus setClosedLoop(bool enabled);
  virtual asynStatus isClosedLoop(bool &closed);
  virtual asynStatus queryPosition();

  virtual asynStatus setPGain(epicsFloat64 value);
  virtual asynStatus setIGain(epicsFloat64 value);
  virtual asynStatus setDGain(epicsFloat64 value);

  inline bool isFlagSet(unsigned int flag) { return (flags_ & flag) == flag; }
  inline void setFlag(unsigned int flag)   { flags_ |= flag; }
  inline void clearFlag(unsigned int flag) { flags_ &= ~flag; }
  inline void setFlag(unsigned int flag, bool set) {
    if (set)
      flags_ |= flag;
    else
      flags_ &= ~flag;
  }

protected:
  virtual asynStatus finished_moving();
  PI3Controller *pc_;      /**< Pointer to the asynMotorController to which this axis belongs.
                              *   Abbreviated because it is used very frequently */
  double encoderPos_;   /**< Cached copy of the encoder position */
  unsigned int flags_;       /**< Cached copy of the current flags */

  bool errored_;
  int axisNum_;         // according to asyn
  char id_;              // axis num according to controller

private:
  friend class PI3Controller;

};

class E816Axis : public PI3Axis
{
public:
  E816Axis(PI3Controller *pC, int axis);

  virtual asynStatus move(double position, int relative, double min_velocity, double max_velocity, double acceleration);
  virtual asynStatus moveVelocity(double min_velocity, double max_velocity, double acceleration);
  virtual asynStatus home(double min_velocity, double max_velocity, double acceleration, int forwards);
  virtual asynStatus stop(double acceleration);
  virtual asynStatus poll(bool *moving);

  virtual asynStatus setClosedLoop(bool enabled);
  virtual asynStatus isClosedLoop(bool &closed);
  virtual asynStatus queryPosition();

  virtual asynStatus setPGain(epicsFloat64 value);
  virtual asynStatus setIGain(epicsFloat64 value);
  virtual asynStatus setDGain(epicsFloat64 value);

  void queryCalibrationParameters();
};

#endif
