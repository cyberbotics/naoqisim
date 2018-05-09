#ifndef HIP_MOTOR_HPP
#define HIP_MOTOR_HPP

// Description: Class for handling the special case of the hip HipYawPitch motors of the Nao robot
//              The LHipYawPitch and RHipYawPitch are in fact a single motor in the real Nao, but
//              they are represented as two distinct Motors (motors) in the Webots models

#include "Device.hpp"
#include <webots/types.h>

namespace Sim {
  class AngleActuator;
  class AngleSensor;
}

class HipMotor : public Device {
public:
  // constructor & destructor
  HipMotor(int step);
  virtual ~HipMotor();

  // reimplemented functions
  virtual void update();

private:
  WbDeviceTag mLeftTag, mRightTag;
  WbDeviceTag mLeftSensorTag;
  const Sim::AngleActuator *mActuator;
  const Sim::AngleSensor *mSensor;

  double mMaxLeftPosition;
  double mMinLeftPosition;
  double mMaxRightPosition;
  double mMinRightPosition;
};

#endif
