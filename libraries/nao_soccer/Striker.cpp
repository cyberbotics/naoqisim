#include "Striker.hpp"
#include <math.h>

using namespace std;

Striker::Striker(int playerID, int teamID, string naoIp, int naoPort) : Player(playerID, teamID, naoIp, naoPort),
  mCalibrating(false), mPreparing(false), mRandomKick(false), mReady(false), mVerification(true), mIterRemaining(0)
{
}

Striker::~Striker() {
}

void Striker::run() {
  if (mPlayerID != 10) // 10: ID for the demonstration, where there's no supervisor
    Player::run();

  int wrongSide = 0;
  int count = 0;
  double epsilon = 0.025;

  while (myStep()) {
    double x, y;   // Store the position of the ball.
    double p1, p2; // Store the position of the two posts of the goal.
    bool ballInFieldOfView;

    getImage();

    if (!mCameraIndex) // top cam is used
      ballInFieldOfView = get_ball_center(mTopVisionManager, mLock, x, y, mImage);
    else // bot cam is used
      ballInFieldOfView = get_ball_center(mBotVisionManager, mLock, x, y, mImage);

    // Part of a mechanism to be able to track the ball from very far: move forward
    // and don't perform any other action for the next 20 iterations of the loop.
    if (mLock && !mIterRemaining) {
      mLastX = x;
      mIterRemaining = 20;
      mMotionProxy->moveToward(1.0f, 0.0f, 0.15f * (float)x);
    }

    else if (mIterRemaining) {
      if (ballInFieldOfView)
        mLastX = x;
      --mIterRemaining;
      mMotionProxy->moveToward(1.0f, 0.0f, 0.15f * (float)x);
      if (!mIterRemaining)
        mLock = false;
    }

    // shoot
    else if (mReady) {
      shoot();

      mReady = false;
    }

    // so as to avoid kicking the ball with the corner of the foot and
    // thus sending it in an random direction, we have to calibrate
    else if (mCalibrating && ballInFieldOfView) {
      const double pos = 0.225;

      if (::fabs(pos - x) > epsilon) {
        if (pos < x)
          mMotionProxy->moveToward(0.0f, 0.7f, 0.0f);
        else
          mMotionProxy->moveToward(0.0f, -0.7f, 0.0f);
      } else {
        mCalibrating = false;
        if (y < -0.25) { // if the ball/robotis-op2 moved too much during the process, start again
          mMotionProxy->stopMove();
          mHeadPitchPos = 0.0;
          mMotionProxy->angleInterpolation("HeadPitch", mHeadPitchPos, 1.0, true);
          if (mVerification) {
            if (mCameraIndex) {
              switchCamera(); // we will use the top cam to look for the goal.
              getImage();
            }
            // This verification is necessary in case the lateral movement wasn't straight
            if (!(get_goal_position(mTopVisionManager, wrongSide, p1, p2, mImage) && (p1 > 0.1 && p2 < -0.1))) {
              mPreparing = true;
              if (p2 >= -0.1)
                mMotionProxy->moveToward(0.0f, -0.7f, 0.3f);
              else
                mMotionProxy->moveToward(0.0f, 0.7f, -0.3f);
            } else {
              mReady = true;
            }
          } else {
            mVerification = true;
            mReady = true;
          }
        }
      }
    }

    // The ball can't be kick to the goal in one hit
    else if (mRandomKick) {
      if (get_goal_position(mTopVisionManager, wrongSide, p1, p2, mImage) || wrongSide) {
        // Actually, it can
        if (!wrongSide && (p1 > 0.1 && p2 < -0.1)) {
          mMotionProxy->stopMove();
          mHeadPitchPos = MAX_ANGLE;
          mMotionProxy->angleInterpolation("HeadPitch", mHeadPitchPos, 1, true);

          if (!mCameraIndex)
            switchCamera(); // we will use the bot camera to look for the ball
          mRandomKick = false;
          mCalibrating = true;
        }
      } else {
        // Kick it so as to replace it where it can maybe be shot in the goal directly
        mHeadPitchPos = MAX_ANGLE;
        mMotionProxy->angleInterpolation("HeadPitch", mHeadPitchPos, 1, true);

        if (!mCameraIndex)
          switchCamera(); // we will use the bot camera to look for the ball
        mRandomKick = false;
        mCalibrating = true;
        mVerification = false;
      }
    }

    // Continue turning around the ball until being in front of the goal
    else if (mPreparing) {
      if (get_goal_position(mTopVisionManager, wrongSide, p1, p2, mImage) && (p1 > 0.1 && p2 < -0.1)) {
        mMotionProxy->stopMove();
        mHeadPitchPos = MAX_ANGLE;
        mMotionProxy->angleInterpolation("HeadPitch", mHeadPitchPos, 1, true);

        if (!mCameraIndex)
          switchCamera(); // we will use the bot camera to look for the ball
        mPreparing = false;
        mCalibrating = true;
      } else if (wrongSide) {
        mPreparing = false;
        mRandomKick = true;
        if (p2 == 1)
          mMotionProxy->moveToward(0.0f, -0.7f, 0.3f);
        else
          mMotionProxy->moveToward(0.0f, 0.7f, -0.3f);
      }
    }

    // if the ball is in the field of view,
    // go in the direction of the ball
    else if (ballInFieldOfView) {
      mLastX = x;
      mMotionProxy->moveToward(1.0f, 0.0f, 0.15f * (float)x);
      if (!mCameraIndex) { // nao sees the ball with the top camera: go quickly
        if (((double) mHeadPitchPos > SWITCH_CAM_1) && (y < -0.35)) { // switch to bot camera if we get close to the ball
          mHeadPitchPos = 0.0;
          mMotionProxy->angleInterpolation("HeadPitch", mHeadPitchPos, 1.0, true);
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
        if ((y < -0.4) && (((double) mHeadPitchPos) >= (0.98 * MAX_ANGLE))) { // ball close to Nao
          mMotionProxy->stopMove();
          mHeadPitchPos = 0.0;
          mMotionProxy->angleInterpolation("HeadPitch", mHeadPitchPos, 1.0, true);
          switchCamera(); // we will use the top cam to look for the goal.
          getImage();
          mPreparing = true;

          // the ball is in front of Nao, check if the goal is
          // in the same direction, turn around the ball if not
          if (!get_goal_position(mTopVisionManager, wrongSide, p1, p2, mImage) || !(p1 > 0.1 && p2 < -0.1)) {
            if (p2 >= -0.1)
              mMotionProxy->moveToward(0.0f, -0.7f, 0.3f);
            else
              mMotionProxy->moveToward(0.0f, 0.7f, -0.3f);
          }
        }
      }
    }

    // the ball is not in the field of view,
    // search it by turning round and moving vertically the head
    else {
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

      if (mCalibrating) {
        mCalibrating = false;
        mVerification = true;
      }
    }
    ++count;
  }
}

// This function is called if the robot has fallen, it will reset all
// progress that Nao made towards shooting the ball as the situation
// has most likely changed.
void Striker::resetProgress() {
  if (mCameraIndex)
    switchCamera();
  mLock = false;
  mCalibrating = false;
  mPreparing = false;
  mRandomKick = false;
  mReady = false;
  mVerification = true;
  mIterRemaining = 0;
  mLastX = 0;
}
