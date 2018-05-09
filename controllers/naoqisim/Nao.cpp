#include "Nao.hpp"
#include "Singletons.hpp"
#include "Camera.hpp"
#include "Motor.hpp"
#include "HipMotor.hpp"
#include "Hand.hpp"
#include "Fsr.hpp"
#include "Bumper.hpp"
#include "InertialUnit.hpp"
#include "Sonar.hpp"
#include "Timer.hpp"

#include <webots/robot.h>

#include <iostream>
#include <list>
#include <cmath>
#include <assert.h>

using namespace std;
using namespace Sim;

Nao::Nao(int timeStep, bool useCameras) {
  // simulation step in milliseconds
  mTimeStep = timeStep;

  // get Nao model from simulation SDK
  Model *model = Singletons::model();

  // create cameras
  if (useCameras) {
    cout << "Webots cameras enabled.\n";
    cout << "Add the '-nocam' option in 'controllerArgs' to disable the cameras and increase the simulator performance.\n";

    vector<const CameraSensor*> cameraSensors = model->cameraSensors();
    for (vector<const CameraSensor*>::const_iterator it = cameraSensors.begin(); it != cameraSensors.end(); ++it)
      mDevices.push_back(new Camera(*it, mTimeStep));
  }

  // create motors
  vector<const AngleActuator*> angleActuators = model->angleActuators();
  for (vector<const AngleActuator*>::const_iterator it = angleActuators.begin(); it != angleActuators.end(); ++it) {
    if ((*it)->name() != "LHipYawPitch" && (*it)->name() != "RHipYawPitch")
      mDevices.push_back(new Motor(*it, mTimeStep));
  }

  // special case for hip motor
  if (model->angleActuator("LHipYawPitch"))
    mDevices.push_back(new HipMotor(mTimeStep));

  // create hands
  vector<const CoupledActuator*> coupledActuators = model->coupledActuators();
  for (vector<const CoupledActuator*>::const_iterator it = coupledActuators.begin(); it != coupledActuators.end(); ++it)
    mDevices.push_back(new Hand(*it, mTimeStep));

  // FSRs
  vector<const FSRSensor*> fsrSensors = model->fsrSensors();

  vector<const FSRSensor*> leftFsrSensors;
  vector<const FSRSensor*> rightFsrSensors;

  for (vector<const FSRSensor*>::const_iterator it = fsrSensors.begin(); it != fsrSensors.end(); ++it) {
    const FSRSensor *fsr = *it;
    if (!fsr->name().substr(0, 5).compare("LFoot"))
      leftFsrSensors.push_back(fsr);
    else if (!fsr->name().substr(0, 5).compare("RFoot"))
      rightFsrSensors.push_back(fsr);
    else
      assert(0);
  }

  assert(leftFsrSensors.size() == 4);
  assert(rightFsrSensors.size() == 4);

  mDevices.push_back(new Fsr(leftFsrSensors, mTimeStep, "LFsr"));
  mDevices.push_back(new Fsr(rightFsrSensors, mTimeStep, "RFsr"));

  // Bumpers
  vector<const BumperSensor*> bumperSensors = model->bumperSensors();
  for (vector<const BumperSensor*>::const_iterator it = bumperSensors.begin(); it != bumperSensors.end(); ++it)
    mDevices.push_back(new Bumper(*it, mTimeStep));

  // Inertial Unit
  vector<const InertialSensor*> inertialSensors = model->inertialSensors();
  for (vector<const InertialSensor*>::const_iterator it = inertialSensors.begin(); it != inertialSensors.end(); ++it)
    mDevices.push_back(new InertialUnit(*it, mTimeStep));

  // Only the receiving sonars (2 and 4) should appear in this list
  vector<const SonarSensor*> sonarSensors = model->sonarSensors();
  for (vector<const SonarSensor*>::const_iterator it = sonarSensors.begin(); it != sonarSensors.end(); ++it)
    mDevices.push_back(new Sonar(*it, mTimeStep));
}

Nao::~Nao() {
  for (unsigned int i = 0; i < mDevices.size(); i++)
    delete mDevices[i];
}

void Nao::run() {
  // main loop
  while (wb_robot_step(mTimeStep) != -1)
    update();
}

void Nao::update() {
  checkRealTime();
  for (unsigned int i = 0; i < mDevices.size(); i++)
    mDevices[i]->update();
}

void Nao::checkRealTime() {
  static Timer timer;
  static double previousVirtualTime = 0;
  static int slice = 0;
  static double timeGap = 10.0; // seconds

  if (timer.delta() >= timeGap) { // every 10 real seconds
    double virtualTime = wb_robot_get_time();
    double speedometer = (virtualTime - previousVirtualTime) / timeGap;

    // check that the second slice is in real-time
    if (slice == 1 && (speedometer > 1.1 || speedometer < 0.9))
      cerr <<
        "The real-time has not been guaranteed during the first 10 second (speedometer = " <<
        speedometer << "). The robot behavior may differ from the expected one." << endl;

    slice++;
    previousVirtualTime = virtualTime;
    timer.reset();
  }
}
