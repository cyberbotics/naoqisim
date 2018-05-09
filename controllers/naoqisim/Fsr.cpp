#include "Fsr.hpp"
#include "LowPassFilter.hpp"
#include "Singletons.hpp"
#include "util.hpp"
#include <webots/robot.h>
#include <webots/touch_sensor.h>
#include <iostream>
#include <assert.h>

using namespace Sim;
using namespace std;
using namespace util;

static const double TO_KILOGRAMS = 1.0 / 9.81;

Fsr::Fsr(vector<const FSRSensor*> sensors, int step, string name) : Device(name) {

  mFSRSensors = sensors;

  mTag = wb_robot_get_device(name.c_str());
  if (! mTag)
    cerr << "Webots device not found for Sim::FSRSensor: " << name << "\n";
  else
    wb_touch_sensor_enable(mTag, step);

  for (int i = 0; i < 4; ++i) {
    mFilter[i] = new LowPassFilter(step, 60);
  }
}

Fsr::~Fsr() {
}

void Fsr::update() {
  if (! mTag) return;

  inferValues();

  int i(0);
  for (vector<const FSRSensor*>::const_iterator it = mFSRSensors.begin(); it != mFSRSensors.end(); ++it) {
    if (! Singletons::hal()->sendFSRSensorValue(*it, (float) mFilter[i]->filteredValue())) {
      cerr << "Sim::HALInterface::sendFSRSensorValue() failed.\n";
    }
    ++i;
  }
}

void Fsr::inferValues() {
  const double *fsv = wb_touch_sensor_get_values(mTag);
  double val[4];

  // The coefficients were calibrated against the real
  // robot so as to obtain realistic sensor values.
  val[0] = fsv[2]/3.4 + 1.5*fsv[0] + 1.15*fsv[1]; // FL
  val[1] = fsv[2]/3.4 + 1.5*fsv[0] - 1.15*fsv[1]; // FR
  val[2] = fsv[2]/3.4 - 1.5*fsv[0] + 1.15*fsv[1]; // RL
  val[3] = fsv[2]/3.4 - 1.5*fsv[0] - 1.15*fsv[1]; // RR

  int i(0);
  for (i = 0; i < 4; ++i) {
    val[i] = clamp(val[i], 0, 25) * TO_KILOGRAMS;
    mFilter[i]->appendRawValue(val[i]);
  }
}
