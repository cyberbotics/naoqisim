#ifndef NAO_HPP
#define NAO_HPP

// Description: Class that represents a single Nao robot and its devices

class Device;

#include <vector>

class Nao {
public:
  // create nao robot
  Nao(int timeStep, bool useCamera);
  virtual ~Nao();

  // run synchronization between simulated robot and HAL
  // this function returns when Webots simulation is terminated by the user
  void run();

private:
  int mTimeStep;
  std::vector<Device*> mDevices;

  void update();
  void checkRealTime();
};

#endif
