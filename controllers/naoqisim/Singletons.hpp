#ifndef SINGLETONS_HPP
#define SINGLETONS_HPP

// Description: static environment for alsim
//              the Model contains a description of the robot model
//              the HALInterface is used to communicate with the sensors and actuators

#include <alnaosim/alnaosim.h>
#include <alrobotmodel/alrobotmodel.h>
#include <string>

namespace Sim {
 class SimLauncher;
}

using namespace Sim;
using namespace std;

class Process;

class Singletons {
public:
  // initialize and shutdown simulation environment
  static bool initialize(const string &model, int naoqiPort, void (*halInitializedCallback)());
  static void shutdown();

  // robot model
  static Model *model() { return mModel; }

  // hardware abstraction layer
  static HALInterface *hal() { return mHal; }

  // dump model info on cout
  static void dumpModel();

private:
  static Model *mModel;
  static HALInterface *mHal;
  static Sim::SimLauncher *mLauncher;
};

#endif
