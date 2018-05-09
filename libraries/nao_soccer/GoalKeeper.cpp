#include "GoalKeeper.hpp"
#include <math.h>

using namespace std;

GoalKeeper::GoalKeeper(int playerID, int teamID, string naoIp, int naoPort) : Player(playerID, teamID, naoIp, naoPort) {

}

GoalKeeper::~GoalKeeper() {
}

void GoalKeeper::run() {
  Player::run();

  bool lock = false;
  int count = 0;

  while (myStep()) {
    lock = false;
    bool ballInFieldOfView;
    double x, y;

    getImage();

    if (!mCameraIndex) // top cam is used
      ballInFieldOfView = get_ball_center(mTopVisionManager, mLock, x, y, mImage);
    else // bot cam is used
      ballInFieldOfView = get_ball_center(mBotVisionManager, mLock, x, y, mImage);

    // if the ball is in the field of view and close enough, look at it, kick it if it's near.
    if (ballInFieldOfView && !lock) {
      mLastX = x;
      mMotionProxy->moveToward(0.0f, 0.0f, 0.15f * (float)x);
      if (!mCameraIndex) { // nao sees the ball with the top camera: go quickly
        if (((double) mHeadPitchPos > SWITCH_CAM_1) && (y < -0.35)) { // switch to bot camera if we get close to the ball
          mHeadPitchPos = 0.0;
          mMotionProxy->angleInterpolation("HeadPitch", mHeadPitchPos, 0.5, true);
          switchCamera();
        } else {
          double offset = (double) mHeadPitchPos - ANGLE_COEFF * y;
          mHeadPitchPos = offset < 0 ? 0: offset > MAX_ANGLE? MAX_ANGLE: offset;
          mMotionProxy->setAngles("HeadPitch", mHeadPitchPos, 1);
        }
      } else {
        double offset = (double) mHeadPitchPos - ANGLE_COEFF * y;
        mHeadPitchPos = offset < 0 ? 0: offset > MAX_ANGLE? MAX_ANGLE: offset;
        mMotionProxy->setAngles("HeadPitch", mHeadPitchPos, 1);

        if ((y < -0.35) && (((double) mHeadPitchPos) >= (0.98 * MAX_ANGLE))) { // ball close to Nao -> shoot
          mMotionProxy->stopMove();

          shoot();
        }
      }
    }

    // otherwise, try to find it
    else {
      if (ballInFieldOfView) {
        mLastX = x;
      }
      if (mLastX > 0)
        mMotionProxy->moveToward(0.0f, 0.0f, 0.3f);
      else
        mMotionProxy->moveToward(0.0f, 0.0f, -0.3f);
      mHeadPitchPos = OSC_ANGLE + (OSC_ANGLE * sin(MAX_ANGLE * count));
      mMotionProxy->setAngles("HeadPitch", mHeadPitchPos, 1);
      if (mCameraIndex && ((double) mHeadPitchPos < SWITCH_CAM_2)) // switch to top camera
        switchCamera();
      if (!mCameraIndex && ((double) mHeadPitchPos > SWITCH_CAM_1)) // switch to bottom camera
        switchCamera();
    }
    ++count;
  }
}
