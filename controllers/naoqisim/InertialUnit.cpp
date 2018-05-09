#include "InertialUnit.hpp"
#include "LowPassFilter.hpp"
#include "Singletons.hpp"
#include <webots/robot.h>
#include <webots/inertial_unit.h>
#include <webots/accelerometer.h>
#include <webots/gyro.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace Sim;
using namespace std;

InertialUnit::InertialUnit(const InertialSensor *sensor, int step) : Device("InertialUnit") {

  mSensor = sensor;

  mInertialUnit = wb_robot_get_device("inertial unit");
  if (mInertialUnit)
    wb_inertial_unit_enable(mInertialUnit, step);

  mAccelerometer = wb_robot_get_device("accelerometer");
  if (mAccelerometer)
    wb_accelerometer_enable(mAccelerometer, step);

  mGyroscope = wb_robot_get_device("gyro");
  if (mGyroscope)
    wb_gyro_enable(mGyroscope, step);

  for (int i=0; i<7; i++)
    mFilters[i] = new LowPassFilter(step, 60);
}

InertialUnit::~InertialUnit() {
  for (int i=0; i<7; i++)
    delete mFilters[i];
}

void InertialUnit::update() {
  if (! mInertialUnit && ! mAccelerometer && ! mGyroscope)
    return;

  // unknown values
  double angleX = 0.0;
  double angleY = 0.0;
  double accX = 0.0;
  double accY = 0.0;
  double accZ = 0.0;
  double gyrX = 0.0;
  double gyrY = 0.0;

  if (mInertialUnit) {
    // roll/pitch/yaw angles in rad
    const double *rpy = wb_inertial_unit_get_roll_pitch_yaw(mInertialUnit);
    angleX = rpy[0];  // roll
    angleY = -rpy[1];  // -pitch
  }

  if (mAccelerometer) {
    // linear acceleration in m/s^2
    const double *acc = wb_accelerometer_get_values(mAccelerometer);
    accX = acc[0];
    accY = acc[1];
    accZ = acc[2];
  }

  if (mGyroscope) {
    // angular velocity in rad/s
    const double *angVel = wb_gyro_get_values(mGyroscope);
    gyrX = angVel[0];
    gyrY = angVel[1];
  }

  // filter values
  mFilters[ANGLE_X]->appendRawValue(angleX);
  mFilters[ANGLE_Y]->appendRawValue(angleY);
  mFilters[ACC_X]->appendRawValue(accX);
  mFilters[ACC_Y]->appendRawValue(accY);
  mFilters[ACC_Z]->appendRawValue(accZ);
  mFilters[GYR_X]->appendRawValue(gyrX);
  mFilters[GYR_Y]->appendRawValue(gyrY);

  // yb: this is the data order expected by HAL according to the dummysim.cpp example
  // in case there was a doubt, the order can be checked by using "monitor" and following keys in the memory viewer
  vector<float> values;
  values.push_back((float) mFilters[ANGLE_X]->filteredValue());  // Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value
  values.push_back((float) mFilters[ANGLE_Y]->filteredValue());  // Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value
  values.push_back((float) mFilters[ACC_X]->filteredValue());  // Device/SubDeviceList/InertialSensor/AccelerometerX/Sensor/Value
  values.push_back((float) mFilters[ACC_Y]->filteredValue());  // Device/SubDeviceList/InertialSensor/AccelerometerY/Sensor/Value
  values.push_back((float) mFilters[ACC_Z]->filteredValue());  // Device/SubDeviceList/InertialSensor/AccelerometerZ/Sensor/Value
  values.push_back((float) mFilters[GYR_X]->filteredValue());  // Device/SubDeviceList/InertialSensor/GyroscopeX/Sensor/Value
  values.push_back((float) mFilters[GYR_Y]->filteredValue());  // Device/SubDeviceList/InertialSensor/GyroscopeY/Sensor/Value

  if (! Singletons::hal()->sendInertialSensorValues(mSensor, values))
    cerr << "Sim::HALInterface::sendInertialSensorValues() failed.\n";
}
