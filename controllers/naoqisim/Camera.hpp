#ifndef CAMERA_HPP
#define CAMERA_HPP

// Description: Class that simulates one camera of the robot

#include "Device.hpp"
#include <webots/types.h>

namespace Sim {
  class CameraSensor;
}

using namespace Sim;
using namespace std;

class Camera : public Device {
public:
  // constructor and destructor
  Camera(const CameraSensor *sensor, int timeStep);
  virtual ~Camera();

  // reimplemented functions
  virtual void update();

private:
  const CameraSensor *mSensor;
  WbDeviceTag mTag;
  int mWidth, mHeight;
};

#endif
