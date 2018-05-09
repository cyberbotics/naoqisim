#include "Sonar.hpp"
#include "Singletons.hpp"
#include <webots/robot.h>
#include <webots/distance_sensor.h>
#include <iostream>

Sonar::Sonar(const SonarSensor *sensor, int step) : Device(sensor->name()) {

  mSensor = sensor;

  mTag = wb_robot_get_device(name().c_str());
  if (! mTag)
    cerr << "Webots DistanceSensor not found for Sim::SonarSensor: " << name() << "\n";
  else
    wb_distance_sensor_enable(mTag, step);
}

Sonar::~Sonar() {
}

void Sonar::update() {

  if (! mTag) return;

  if (mSensor) {
    float value = (float)wb_distance_sensor_get_value(mTag);

    // send 10 echoe values, only the 1st one is used by the HAL
    if (! Singletons::hal()->sendSonarSensorValues(mSensor, vector<float>(10, value)))
      cerr << "Sim::HALInterface::sendSonarSensorValues() failed.\n";
  }
}
