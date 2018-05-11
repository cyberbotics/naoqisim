#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP

/**
 * Description: Class resolving the Nao Challenge. Coordinates vision
 *              and movements to resolve each of the three tasks.
 */

#include "MovementManager.hpp"
#include "VisionManager.hpp"
#include <alcommon/albroker.h>

#define MAX_RUN 4
#define MAX_CALIBRATION_ATTEMPT 3
#define MAX_SIDESTEP_ATTEMPT 15

class TaskManager {
public:
  TaskManager(std::string naoIp, int naoPort);
  ~TaskManager();
  void run();

private:
  MovementManager *mMovementManager;
  VisionManager *mVisionManager;
  boost::shared_ptr<AL::ALBroker> mBroker;

  void moveKeyInPlace();
  void turnDistributorOn();
  void pointCalendar();
  void searchLine();
  void calibration(double epsilon, double offsetRatio, std::string item, bool align);
  void approachColor(double visionRequired, double velocity, bool correction);
  void searchItem(std::string item);
  bool findItem(std::string item);
  double followLine();
};
#endif
