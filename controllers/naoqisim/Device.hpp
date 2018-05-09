#ifndef DEVICE_HPP
#define DEVICE_HPP

// Description: Abstract base class for all simulated devices in this project

#include <string>

using namespace std;

class Device {
public:
  // constructor
  Device(const string &name) { mName = name; }

  // destructor
  virtual ~Device() {}

  // device name as in HAL
  const string &name() const { return mName; }

  // update the device: Webots -> HAL and HAL -> Webots
  virtual void update() = 0;

private:
  string mName;
};

#endif
