#ifndef PLAYER_HPP
#define PLAYER_HPP

//-----------------------------------------------------------------------------
//  Description:       Base class for Striker, Defender and GoalKeeper
//  Project:           Robotstadium, the online robot soccer competition
//-----------------------------------------------------------------------------

#include <vm_wrapper.h>
#include <RoboCupGameControlData.h>
#include <alcommon/albroker.h>
#include <alproxies/alledsproxy.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/alrobotpostureproxy.h>
#include <alproxies/alvideodeviceproxy.h>

// We define the 4 variables due to an issue sending similar angles from naoqi on Windows
#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>         /* definition of struct sockaddr_in */
#include <netdb.h>              /* definition of gethostbyname */
#include <arpa/inet.h>          /* definition of inet_ntoa */
#include <unistd.h>             /* definition of close */
#endif

#define TOP_CAM 0
#define BOT_CAM 1

// There is currently an issue with the angles, the values
// specified through the NAOqi API depend on the simulation speed
// The following are for a simulation speed of 1. If this speed
// cannot be reached, the behavior should still work, but in
// case it does not, these values must be raised.
#define MAX_ANGLE 0.4
#define OSC_ANGLE 0.2
#define SWITCH_CAM_1 0.32
#define SWITCH_CAM_2 0.16
#define ANGLE_COEFF 0.15

#define MAX_IMG_TRY 3

class Player {
public:
  Player(int playerID, int teamID, std::string naoIp, int naoPort);
  virtual ~Player();
  virtual void run();

protected:
  int mLock;
  int mPlayerID;
  int mTeamID;
  int mCameraIndex;
  int mColorSpace; // we want the same for both cameras
  int mFps;        // same thing
  int mResolution;
  int mWidth;
  int mHeight;
  double mLastX;
  unsigned char *mImage;
  struct RoboCupGameControlData *mGameControlData;
  class VisionManager *mBotVisionManager;
  class VisionManager *mTopVisionManager;
  boost::shared_ptr<AL::ALLedsProxy> mLedsProxy;
  boost::shared_ptr<AL::ALMotionProxy> mMotionProxy;
  boost::shared_ptr<AL::ALRobotPostureProxy> mPostureProxy;
  boost::shared_ptr<AL::ALVideoDeviceProxy> mTopCameraProxy;
  boost::shared_ptr<AL::ALVideoDeviceProxy> mBotCameraProxy;
  AL::ALValue mHeadPitchPos;
  AL::ALValue mNames;
  AL::ALValue mAngleLists;
  AL::ALValue mTimeLists;
  std::string mNameTop;
  std::string mNameBot;

  void getImage();
  void switchCamera();
  void shoot();
  virtual void resetProgress();
  int myStep();

private:
  int fd;
  fd_set rfds;

  void readIncomingMessages();
  void checkIfFallen();
  void updateGameControl();
  void initializeShootMotion();
  bool isBlue() const;
  bool isRed() const;
  int accept_client(int server_fd);
  int create_socket_server(int port);
};

#endif
