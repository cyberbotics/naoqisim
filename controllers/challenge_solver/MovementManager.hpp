#ifndef MOVEMENT_MANAGER_HPP
#define MOVEMENT_MANAGER_HPP

/**
 * Description: Contains the functions for moving Nao.
 */

#include <alproxies/almotionproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/alrobotpostureproxy.h>
#include <alvalue/alvalue.h>

class MovementManager {
public:
  MovementManager(boost::shared_ptr<AL::ALBroker> broker);
  ~MovementManager();
  void searchOnGround();
  void endSearch();
  void walk(double velocity, double direction);
  void stopMoving();
  void turn(double angle);
  void calibrate(double offset);
  void look(std::string direction);
  void alignWithObstacle();
  void prepareArm();
  void pickupKey();
  void handleKey();
  void prepareKey();
  void releaseKey();
  void pressButton();
  void pointFist(double verticalAngle);
  void followupTask();
  bool obstacleDetected();

private:
  int mSearchGround;
  int mRotationCnt;
  AL::ALValue mHeadPitchPos;
  boost::shared_ptr<AL::ALMotionProxy> mMotionProxy;
  boost::shared_ptr<AL::ALMemoryProxy> mMemoryProxy;
  boost::shared_ptr<AL::ALRobotPostureProxy> mPostureProxy;
};
#endif
