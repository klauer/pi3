// vi: sw=2 ts=2 sts=2 expandtab
#ifndef _PI3_CONTROLLER_H
#define _PI3_CONTROLLER_H

#define PI3_TIMEOUT              0.5
#define PI3_STRING_SIZE          160 // no clue what error messages to expect...
#define PI3_UNIT_SCALE           1.0e6

#define PI3_STATUS_STRING        "PI3_STATUS_STRING"

// #define PI3_ERRORS_PARAM         "Parameter error in command: "

// static const char *PI3_IGNORE_STRINGS[] = { "PI33CL>", NULL };

static const char* driverName = "PI3";

/* NOTE on unitScale_:
 * Trying to use 1.0 for the encoder/position resolution does not work,
 * regardless of if your encoder gives floating point position readout.
 *
 * It will result in truncation of the values in EPICS, such that you'd
 * only see 0.5, if in fact the motor was at 0.5345423. As such, the position
 * is kept properly internally, but it is scaled when being passed back
 * and forth from EPICS.
 */

#include "pi3axis.h"

class PI3Controller : public asynMotorController {
public:
  PI3Controller(const char *portName, const char *PI3PortName, int numAxes, double pollPeriod,
                const char *input_eos="\r", const char *output_eos="\r");
  virtual PI3Axis* getAxis(int axisNo) {
    return (PI3Axis*)asynMotorController::getAxis(axisNo);
  }
  int getAxisCount() { return numAxes_; }

  virtual asynStatus write(const char* fmt, ...);
  virtual asynStatus write(const char* fmt, va_list);
  virtual asynStatus writeRead(char* input, size_t* nread, const char* fmt, ...);
  virtual asynStatus writeRead(char* input, size_t* nread, const char* fmt, va_list);
  virtual asynStatus writeReadInt(int &value, const char* fmt, ...);
  virtual asynStatus writeReadDouble(double &value, const char* fmt, ...);
  virtual asynStatus writeReadDouble(double &value, const char* fmt, va_list);
  virtual asynStatus queryPositions();
  virtual asynStatus queryVersion();

  double getUnitScale() {
      return unitScale_;
  }

  asynUser *getAsynUser() {
      return pasynUser_;
  }

protected:
  virtual asynStatus checkError();

private:
  friend class PI3Axis;
  friend class E816Axis;

  asynUser *pasynUser_;
  char outString_[PI3_STRING_SIZE];

#define FIRST_PI3_PARAM PI3StatusString_
  int PI3StatusString_;
#define LAST_PI3_PARAM PI3StatusString_
#define NUM_PI3_PARAMS (&LAST_PI3_PARAM - &FIRST_PI3_PARAM + 1)
  double unitScale_; // see note above
  double timeout_;

};


class E816Controller : public PI3Controller {
public:
  E816Controller(const char *portName, const char *PI3PortName, int numAxes, double pollPeriod);
  virtual E816Axis* getAxis(int axisNo) {
    return (E816Axis*)asynMotorController::getAxis(axisNo);
  }

private:
  friend class E816Axis;
};

/* Use the following structure and functions to manage multiple instances
 * of the driver */
typedef struct PI3Node {
  ELLNODE node;
  const char *portName;
  PI3Controller *pController;
} PI3Node;

bool addToList(const char *portName, PI3Controller *drv);
PI3Controller* findByPortName(const char *portName);

#endif
