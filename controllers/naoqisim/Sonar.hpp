#ifndef SONAR_HPP
#define SONAR_HPP

// Description: Sonar sensor

#include "Device.hpp"
#include <webots/types.h>

namespace Sim {
  class SonarSensor;
}

using namespace Sim;
using namespace std;

class Sonar : public Device {
public:
  // constructor & destructor
  Sonar(const SonarSensor *sensor, int step);
  virtual ~Sonar();

  // reimplemented functions
  virtual void update();

private:
  WbDeviceTag mTag;
  const SonarSensor *mSensor;
};

#endif
