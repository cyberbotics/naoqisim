#include "MovementManager.hpp"
#include "const.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;
using namespace AL;

MovementManager::MovementManager(boost::shared_ptr<AL::ALBroker> broker) {
  mMotionProxy = boost::shared_ptr<ALMotionProxy> (new ALMotionProxy(broker));
  mMemoryProxy = boost::shared_ptr<ALMemoryProxy> (new ALMemoryProxy(broker));
  mPostureProxy = boost::shared_ptr<ALRobotPostureProxy> (new ALRobotPostureProxy(broker));
  mSearchGround = 0;
  mRotationCnt = 0;
  while (!mMotionProxy->robotIsWakeUp())
    mMotionProxy->wakeUp();
  mPostureProxy->goToPosture("Stand", 1.0);
  mMotionProxy->moveInit();
}

MovementManager::~MovementManager() {
  stopMoving();
}

void MovementManager::searchOnGround() {
  if (!(bool) mMotionProxy->getStiffnesses("RHipRoll")[0])
    throw logic_error("Nao has fallen.");

  if (!mSearchGround) {
    mHeadPitchPos = ANGLE_BOT;
    mMotionProxy->angleInterpolation("HeadPitch", mHeadPitchPos, 0.5, true);
    mSearchGround = 1;
    // sleep to avoid scanning the ground before the movement is over.
#ifdef _WIN32
      Sleep(500);
#else
      usleep(500000);
#endif
  } else {
    if (mRotationCnt < 8) {
      turn(0.785);
      ++mRotationCnt;
    } else {
      if ((double) mHeadPitchPos > 0) {
        mHeadPitchPos = (double) mHeadPitchPos - ANGLE_ITER;
        mMotionProxy->angleInterpolation("HeadPitch", mHeadPitchPos, 1, true);
        mRotationCnt = 0;
      } else {
        throw logic_error("The research wasn't successful.");
      }
    }
  }
}

void MovementManager::endSearch() {
  mSearchGround = 0;
  mRotationCnt = 0;
}

void MovementManager::walk(double velocity, double direction) {
  if (!(bool) mMotionProxy->getStiffnesses("RHipRoll")[0])
    throw logic_error("Nao has fallen.");

  mMotionProxy->moveToward(velocity, 0, direction);
}

void MovementManager::stopMoving() {
  mMotionProxy->stopMove();
}

void MovementManager::turn(double angle) {
  mMotionProxy->moveTo(0, 0, angle);
}

void MovementManager::calibrate (double offset) {
  mMotionProxy->moveTo(0, offset, 0);
}

void MovementManager::look(string direction) {
  if (direction == "front") {
    mHeadPitchPos = 0;
    mMotionProxy->angleInterpolation("HeadYaw", 0, 0.8, true);
    mMotionProxy->angleInterpolation("HeadPitch", mHeadPitchPos, 0.8, true);
  } else if (direction == "down") {
    mHeadPitchPos = 0.5;
    mMotionProxy->angleInterpolation("HeadYaw", 0, 0.8, true);
    mMotionProxy->angleInterpolation("HeadPitch", mHeadPitchPos, 0.8, true);
  } else if (direction == "left" || direction == "left+" ||
              direction == "right" || direction == "right+") {
    if (mHeadPitchPos != ALValue(0)) {
      mHeadPitchPos = 0;
      mMotionProxy->angleInterpolation("HeadPitch", mHeadPitchPos, 0.8, true);
    }

    if (direction == "left")
      mMotionProxy->angleInterpolation("HeadYaw", 0.6, 0.7, true);
    else if (direction == "left+")
      mMotionProxy->angleInterpolation("HeadYaw", 1.7, 1.0, true);
    else if (direction == "right")
      mMotionProxy->angleInterpolation("HeadYaw", -0.6, 0.7, true);
    else
      mMotionProxy->angleInterpolation("HeadYaw", -1.7, 1.0, true);
  } else {
    cerr << "Error: function look() in MovementManager expects \
             inputs \"front\", \"down\", \"left[+]\" or \"right[+]\"." << endl;
  }
}

void MovementManager::alignWithObstacle() {
  double epsilon = 0.001;
  double left = mMemoryProxy->getData("Device/SubDeviceList/US/Left/Sensor/Value");
  double right = mMemoryProxy->getData("Device/SubDeviceList/US/Right/Sensor/Value");

  while (abs(left - right) > epsilon) {
    turn(5 * (right - left));
    left = mMemoryProxy->getData("Device/SubDeviceList/US/Left/Sensor/Value");
    right = mMemoryProxy->getData("Device/SubDeviceList/US/Right/Sensor/Value");
  }
}

void MovementManager::prepareArm() {
  bool isAbsolute = true;
  AL::ALValue names;
  AL::ALValue angleLists;
  AL::ALValue timeLists;

  names = AL::ALValue::array("RElbowRoll", "RShoulderPitch", "RShoulderRoll");

  angleLists.arraySetSize(3);
  angleLists[0] = 1.536;
  angleLists[1] = 0.052;
  angleLists[2] = -0.08;

  timeLists.arraySetSize(3);
  timeLists[0] = 2;
  timeLists[1] = 2;
  timeLists[2] = 2;

  mMotionProxy->setMoveArmsEnabled(true, false);

  mMotionProxy->angleInterpolation(names, angleLists, timeLists, isAbsolute);

  mMotionProxy->openHand("RHand");
}

void MovementManager::pickupKey() {
  mMotionProxy->angleInterpolation("RElbowYaw", 0.873, 1.5, true);
  mMotionProxy->closeHand("RHand");

  mMotionProxy->moveTo(-0.4, 0, 0);
  turn(3.1415);
}

void MovementManager::handleKey() {
  bool isAbsolute = true;
  AL::ALValue names;
  AL::ALValue angleLists;
  AL::ALValue timeLists;

  names = AL::ALValue::array("RElbowRoll", "RShoulderPitch", "RShoulderRoll", "RElbowYaw", "RWristYaw");

  angleLists.arraySetSize(5);
  angleLists[0] = 1.56;
  angleLists[1] = 1.4;
  angleLists[2] = -0.3;
  angleLists[3] = 1.39;
  angleLists[4] = -0.1;

  timeLists.arraySetSize(5);
  timeLists[0] = 1;
  timeLists[1] = 1;
  timeLists[2] = 1;
  timeLists[3] = 1;
  timeLists[4] = 1;

  mMotionProxy->angleInterpolation(names, angleLists, timeLists, isAbsolute);
}

void MovementManager::prepareKey() {
  bool isAbsolute = true;
  AL::ALValue names;
  AL::ALValue angleLists;
  AL::ALValue timeLists;

  names = AL::ALValue::array("RWristYaw", "RShoulderPitch");

  angleLists.arraySetSize(2);
  angleLists[0] = -1.32;
  angleLists[1] = 0;

  timeLists.arraySetSize(2);
  timeLists[0] = 0.5;
  timeLists[1] = 0.5;

  mMotionProxy->angleInterpolation(names, angleLists, timeLists, isAbsolute);
}

void MovementManager::releaseKey() {
  mMotionProxy->openHand("RHand");
#ifdef _WIN32
      Sleep(1000);
#else
      usleep(1000000);
#endif
  mMotionProxy->closeHand("RHand");
  mMotionProxy->angleInterpolation("RWristYaw", 0, 0.5, true);
}

void MovementManager::pressButton() {
  bool isAbsolute = true;
  ALValue names;
  ALValue angleLists;
  ALValue timeLists;

  names = ALValue::array("RElbowRoll", "RShoulderPitch", "RShoulderRoll", "RElbowYaw");

  angleLists.arraySetSize(4);
  angleLists[0] = ALValue::array(1.4, 0.47, 1.4);
  angleLists[1] = ALValue::array(-0.23);
  angleLists[2] = ALValue::array(-0.008);
  angleLists[3] = ALValue::array(0.04);

  timeLists.arraySetSize(4);
  timeLists[0] = ALValue::array(2, 2.5, 3);
  timeLists[1] = ALValue::array(2);
  timeLists[2] = ALValue::array(2);
  timeLists[3] = ALValue::array(2);

  mMotionProxy->angleInterpolation(names, angleLists, timeLists, isAbsolute);

  mMotionProxy->setMoveArmsEnabled(true, false);
}

void MovementManager::pointFist(double verticalAngle) {
  bool isAbsolute = true;
  ALValue names;
  ALValue angleLists;
  ALValue timeLists;

  names = ALValue::array("RElbowRoll", "RShoulderPitch", "RShoulderRoll");

  angleLists.arraySetSize(3);
  angleLists[0] = 0.008;
  angleLists[1] = verticalAngle;
  angleLists[2] = -0.008;

  timeLists.arraySetSize(3);
  timeLists[0] = 1.5;
  timeLists[1] = 1.5;
  timeLists[2] = 1.5;

  mMotionProxy->angleInterpolation(names, angleLists, timeLists, isAbsolute);
}

void MovementManager::followupTask() {
#ifdef _WIN32
      Sleep(1000);
#else
      usleep(1000000);
#endif
  mMotionProxy->moveTo(-0.25, 0, 0);
  mMotionProxy->setMoveArmsEnabled(true, true);
  mMotionProxy->moveTo(-0.1, 0, 0);
}

bool MovementManager::obstacleDetected() {
  double left = mMemoryProxy->getData("Device/SubDeviceList/US/Left/Sensor/Value");
  double right = mMemoryProxy->getData("Device/SubDeviceList/US/Right/Sensor/Value");

  return (left < 0.3 && right < 0.3);
}
