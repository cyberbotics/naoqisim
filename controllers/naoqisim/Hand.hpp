#ifndef HAND_HPP
#define HAND_HPP

// Description: Class for controlling a Nao hand

#include "Device.hpp"
#include <webots/types.h>

namespace Sim {
  class CoupledActuator;
  class CoupledSensor;
}

class Hand : public Device {
public:
  // constructor & destructor
  Hand(const Sim::CoupledActuator *actuator, int step);
  virtual ~Hand();

  // reimplemented functions
  virtual void update();

private:
  enum { PHALANX_MAX = 8 };
  WbDeviceTag mTags[PHALANX_MAX];
  const Sim::CoupledActuator *mActuator;
  const Sim::CoupledSensor *mSensor;
};

#endif
