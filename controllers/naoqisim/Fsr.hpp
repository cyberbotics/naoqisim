#ifndef FSR_HPP
#define FSR_HPP

// Description: Simulated Force Sensitive Resistor (FSR) of the Nao robot

#include "Device.hpp"
#include <webots/types.h>
#include <vector>

namespace Sim {
  class FSRSensor;
}

class LowPassFilter;

class Fsr : public Device {
public:
  // constructor and destructor
  Fsr(std::vector<const Sim::FSRSensor*> sensors, int step, std::string name);
  virtual ~Fsr();

  // reimplemented functions
  virtual void update();

private:
  void inferValues();
  WbDeviceTag mTag;
  std::vector<const Sim::FSRSensor*> mFSRSensors;
  LowPassFilter *mFilter[4];
};

#endif
