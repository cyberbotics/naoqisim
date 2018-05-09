#include "Bumper.hpp"
#include "Singletons.hpp"
#include <webots/robot.h>
#include <webots/touch_sensor.h>
#include <iostream>

Bumper::Bumper(const BumperSensor *sensor, int step) : Device(sensor->name()) {

  mSensor = sensor;

  mTag = wb_robot_get_device(sensor->name().c_str());
  if (! mTag)
    cerr << "Webots device not found for Sim::BumperSensor: " << name() << "\n";
  else
    wb_touch_sensor_enable(mTag, step);
}

Bumper::~Bumper() {
}

void Bumper::update() {

  if (! mTag) return;

  double value = wb_touch_sensor_get_value(mTag);
  if (! Singletons::hal()->sendBumperSensorValue(mSensor, (float)value))
    cerr << "Sim::HALInterface::sendBumperSensorValue() failed.\n";
}
