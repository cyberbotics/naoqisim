#ifndef INERTIAL_UNIT_HPP
#define INERTIAL_UNIT_HPP

// Description: Simulate the Nao's inertial unit using Webots Accelerometer, Gyro, and InertialUnit

#include "Device.hpp"
#include <webots/types.h>

namespace Sim {
  class InertialSensor;
}

class LowPassFilter;

class InertialUnit : public Device {
public:
  // constructor & destructor
  InertialUnit(const Sim::InertialSensor *sensor, int step);
  virtual ~InertialUnit();

  // reimplemented functions
  virtual void update();

private:
  enum {
    ANGLE_X = 0, ANGLE_Y,
    ACC_X, ACC_Y, ACC_Z,
    GYR_X, GYR_Y
  };

  WbDeviceTag mAccelerometer, mGyroscope, mInertialUnit;
  const Sim::InertialSensor *mSensor;
  LowPassFilter *mFilters[7];
};

#endif
