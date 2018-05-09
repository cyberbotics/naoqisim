#include "Motor.hpp"
#include "Singletons.hpp"
#include "util.hpp"
#include <webots/motor.h>
#include <webots/position_sensor.h>
#include <webots/robot.h>
#include <iostream>

#ifdef _MSC_VER
#define isnan _isnan
#endif

using namespace util;

Motor::Motor(const AngleActuator *actuator, int step) : Device(actuator->name()) {

  mActuator = actuator;

  mTag = wb_robot_get_device(name().c_str());
  string sensorName = name();
  sensorName.push_back('S');
  mSensorTag = wb_robot_get_device(sensorName.c_str());

  if (! mTag) {
    cerr << "Webots Motor not found for Sim::AngleActuator: " << name() << "\n";
    mMaxPosition = 0.0;
    mMinPosition = 0.0;
  } else {
    mMaxPosition = wb_motor_get_max_position(mTag);
    mMinPosition = wb_motor_get_min_position(mTag);
  }

  if (! mSensorTag)
    cerr << "Webots PositionSensor not found for Sim::AngleSensor: " << name() << "\n";
  else
    wb_position_sensor_enable(mSensorTag, step);

  mSensor = Singletons::model()->angleSensor(name());

  if (! mSensor)
    cerr << "Sim::AngleSensor not found for motor: " << name() << "\n";
}

Motor::~Motor() {
}

void Motor::update() {

  if (! mTag)
    return;

  // set target position
  double target = Singletons::hal()->fetchAngleActuatorValue(mActuator);
  if (isnan(target))
    target = mActuator->startValue();

  // send target position to Webots
  wb_motor_set_position(mTag, clamp(target, mMinPosition, mMaxPosition));

  // effective position feedback
  if (mSensor) {
    double feedback = wb_position_sensor_get_value(mSensorTag);
    if(isnan(feedback))
      return;
    if (! Singletons::hal()->sendAngleSensorValue(mSensor, (float)feedback))
      cerr << "Sim::HALInterface::sendAngleSensorValue() failed.\n";
  }
}
