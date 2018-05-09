#ifndef MOTOR_HPP
#define MOTOR_HPP

// Description: A class for interfacing any of the (active) motor including the wheels (of Pepper)

#include "Device.hpp"
#include <webots/types.h>

namespace Sim {
  class AngleActuator;
  class AngleSensor;
}

using namespace Sim;
using namespace std;

class Motor : public Device {
public:
  // constructor & destructor
  Motor(const AngleActuator *actuator, int step);
  virtual ~Motor();

  // reimplemented functions
  virtual void update();

private:
  WbDeviceTag mTag;
  WbDeviceTag mSensorTag;
  const AngleActuator *mActuator;
  const AngleSensor *mSensor;

  double mMaxPosition;
  double mMinPosition;
};

#endif
