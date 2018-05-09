#include "Player.hpp"
#include <alerror/alerror.h>
#include <alvision/alvisiondefinitions.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#define MAX_NUM_CONNECTIONS 1   /* One connection is needed for the game
                                 * controller, raise this number to also
                                 * allow inter-nao communication with TCP.*/

using namespace std;
using namespace AL;

Player::Player(int playerID, int teamID, string naoIp, int naoPort): mLock(0),
  mPlayerID(playerID), mTeamID(teamID), mCameraIndex(TOP_CAM), mColorSpace(kBGRColorSpace),
  mLastX(0.0), mHeadPitchPos(0)
{
  boost::shared_ptr<AL::ALBroker> broker = AL::ALBroker::createBroker("playerBroker", "0.0.0.0", 54000 + playerID + (5 * teamID), naoIp, naoPort);
  mLedsProxy = boost::shared_ptr<ALLedsProxy>(new ALLedsProxy(broker));
  mMotionProxy = boost::shared_ptr<ALMotionProxy>(new ALMotionProxy(broker));
  mPostureProxy = boost::shared_ptr<ALRobotPostureProxy>(new ALRobotPostureProxy(broker));
  mBotCameraProxy = boost::shared_ptr<ALVideoDeviceProxy> (new ALVideoDeviceProxy(broker));
  mTopCameraProxy = boost::shared_ptr<ALVideoDeviceProxy> (new ALVideoDeviceProxy(broker));
  mGameControlData = new RoboCupGameControlData;
  mGameControlData->state = STATE_INITIAL;

  string pid = static_cast<ostringstream*>( &(ostringstream() << playerID) )->str();
  string tid = static_cast<ostringstream*>( &(ostringstream() << teamID) )->str();
  mNameTop = "Top(" + pid + ", " + tid + ")";
  mNameBot = "Bot(" + pid + ", " + tid + ")";

  mResolution = 0; //mTopCameraProxy->getResolution(mCameraIndex); // currently not supported by simulator-sdk
  mFps = mTopCameraProxy->getFrameRate(mCameraIndex);

  switch(mResolution) {
    case 0: {
      mWidth = 160;
      mHeight = 120;
      break;
    }
    case 1: {
      mWidth = 320;
      mHeight = 240;
      break;
    }
    case 2: {
      mWidth = 640;
      mHeight = 480;
      break;
    }
    case 3: {
      mWidth = 1280;
      mHeight = 960;
      break;
    }
    case 7: {
      mWidth = 80;
      mHeight = 60;
      break;
    }
    case 8: {
      mWidth = 40;
      mHeight = 30;
      break;
    }
    default: {
      throw logic_error("Camera resolution not handled.");
    }
  }

  mImage = (unsigned char *) malloc(3 * mWidth * mHeight * sizeof(unsigned char));

  mTopVisionManager = create_vision_manager(mWidth, mHeight, false);
  mBotVisionManager = create_vision_manager(mWidth, mHeight, false);

  mNameTop = mTopCameraProxy->subscribeCamera(mNameTop, TOP_CAM, mResolution, mColorSpace, mFps);
  mNameBot = mBotCameraProxy->subscribeCamera(mNameBot, BOT_CAM, mResolution, mColorSpace, mFps);

  // "The team color should be displayed during the whole game
  // on the LED of the left foot (blue/red)." (Naoqi doesn't support
  // this for simulated robots at the moment)
  if (isRed())
    mLedsProxy->fadeRGB("LeftFootLeds", 0xff0000, 0);
  else
    mLedsProxy->fadeRGB("LeftFootLeds", 0x0000ff, 0);

  if (mPlayerID != 10) {
    fd = create_socket_server(naoPort + 10000);
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
  }

  initializeShootMotion();
}

Player::~Player() {
  if (mPlayerID != 10) {
    int ret;
  #ifdef _WIN32
    closesocket(fd);
    ret = WSACleanup();
  #else
    ret = close(fd);
  #endif
    if (ret != 0)
        cerr << "Cannot close socket." << endl;
  }

  mTopCameraProxy->unsubscribe(mNameTop);
  mBotCameraProxy->unsubscribe(mNameBot);

  delete_vision_manager(mTopVisionManager);
  delete_vision_manager(mBotVisionManager);
}

void Player::run() {
  while (mGameControlData->state != STATE_PLAYING)
    myStep();
  if (!mMotionProxy->robotIsWakeUp())
    mMotionProxy->wakeUp();
  mMotionProxy->moveInit();
}

int Player::myStep() {
#ifdef _WIN32
  Sleep(250); // may be harmful if the simulation speed is high.
#else
  usleep(250000); // may be harmful if the simulation speed is high.
#endif
  if (mPlayerID != 10)
    readIncomingMessages();
  checkIfFallen();
  return 1;
}

// refreshes the data for the image of the camera currently used (top or bottom one)
void Player::getImage() {
  bool imageOk = false;
  ALValue image;
  for (int i = 0; i < MAX_IMG_TRY && !imageOk; ++i) {
    try {
      if (mCameraIndex)
        image = mBotCameraProxy->getImageRemote(mNameBot);
      else
        image = mTopCameraProxy->getImageRemote(mNameTop);
      imageOk = true;
    } catch (ALError e) {
      if (mCameraIndex) {
        mBotCameraProxy->unsubscribe(mNameBot);
        mNameBot = mBotCameraProxy->subscribeCamera("Challenge_Bot", BOT_CAM, mResolution, mColorSpace, mFps);
      } else {
        mTopCameraProxy->unsubscribe(mNameTop);
        mNameTop = mTopCameraProxy->subscribeCamera("Challenge_Top", TOP_CAM, mResolution, mColorSpace, mFps);
      }
    }
  }

  if (!imageOk)
    throw logic_error("Camera image couldn't be recuperated.");

  memcpy(mImage, image[6].GetBinary(), (int)image[0] * (int)image[1] * (int)image[2]);

  if (mCameraIndex)
    mBotCameraProxy->releaseImage(mNameBot);
  else
    mTopCameraProxy->releaseImage(mNameTop);
}

void Player::switchCamera() {
  mCameraIndex = (mCameraIndex + 1) % 2;
}

void Player::shoot() {
  mMotionProxy->angleInterpolation(mNames, mAngleLists, mTimeLists, true);
}

void Player::resetProgress() {
}

void Player::readIncomingMessages() {
  int n;
  int number;
  char buffer[1024];
  struct timeval tv = { 0, 0 };

  /* Set up the parameters used for the select statement */
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);

  /*
   * Watch TCPIP file descriptor to see when it has input.
   * No wait - polling as fast as possible
   */
  number = select(fd + 1, &rfds, NULL, NULL, &tv);

  /* If there is no data at the socket, then redo loop */
  if (number == 0)
    return;

  /* ...otherwise, there is data to read, so read & process. */
  n = recv(fd, buffer, 1024, 0);
  if (n < 0) {
      cout << "(" << mPlayerID << "," << mTeamID <<  ") got an error reading from socket" << endl;
  }
  buffer[n] = '\0';

  if (memcmp(buffer, GAMECONTROLLER_STRUCT_HEADER, sizeof(GAMECONTROLLER_STRUCT_HEADER) - 1) == 0) {
    // we use only the last data in the queue.
    memcpy(mGameControlData, &buffer[n - sizeof(RoboCupGameControlData)], sizeof(RoboCupGameControlData));
    updateGameControl();
  }
}

// Attempts to stand up after having fallen
void Player::checkIfFallen() {
  if (!(bool) mMotionProxy->getStiffnesses("RHipRoll")[0]) {
    while (!mPostureProxy->goToPosture("StandInit", 1.0));
    resetProgress();
  }
}

// Will not do anything with an old version of the Simulator-SDK for Naoqi
void Player::updateGameControl() {
  // Teams that support the GameController can visualize whether the robot’s
  // team has kick-off on the LED of the right foot (off/white) in the states
  // initial, ready and set.
  if (mTeamID == mGameControlData->kickOffTeam && mGameControlData->state != STATE_PLAYING)
    mLedsProxy->fadeRGB("RightFootLeds", 0xffffff, 0); // white
  else
    mLedsProxy->fadeRGB("RightFootLeds", 0x000000, 0); // black

  // The current game state should be displayed on
  // the LED in the torso. (Done in the eyes instead, the ChestLeds
  // can't be controlled via Naoqi anymore)
  if (mGameControlData->teams[mTeamID].players[mPlayerID].penalty) {
    mLedsProxy->fadeRGB("FaceLeds", 0xff2222, 0); // red
  } else {
    switch (mGameControlData->state) {
      case STATE_INITIAL:
      case STATE_FINISHED:
        mLedsProxy->fadeRGB("FaceLeds", 0x000000, 0); // off
        break;
      case STATE_READY:
        mLedsProxy->fadeRGB("FaceLeds", 0x2222ff, 0); // blue
        break;
      case STATE_SET:
        mLedsProxy->fadeRGB("FaceLeds", 0xffff22, 0); // yellow
        break;
      case STATE_PLAYING:
        mLedsProxy->fadeRGB("FaceLeds", 0x22ff22, 0); // green
        break;
    }
  }
}

// Reads the Shoot motion in the Shoot.motion file and writes the values
// in arrays that can be used with Naoqi.
void Player::initializeShootMotion() {
  ifstream motion("../../motions/Shoot.motion");

  string line("");
  string oldLine("");

  getline(motion, line);

  int n = count(line.begin(), line.end(), ',') - 1;
  int index(0);
  int nextIndex(line.find(",", index));
  // [index, nextIndex[ is the first header (#WEBOTS_MOTION), we skip it

  index = nextIndex + 1;
  nextIndex = line.find(",", index);
  // [index, nextIndex[ is the second header (V1.0), we skip it

  index = nextIndex + 1;
  nextIndex = line.find(",", index);
  // [index, nextIndex[ is the first element, we need it to
  // initialize the joint names array

  mNames = ALValue::array(line.substr(index, nextIndex - index));

  // We now add the following elements to the array
  index = nextIndex + 1;
  for (int i = 1; i < n; ++i) {
    nextIndex = line.find(",", index);
    mNames.arrayPush(line.substr(index, nextIndex - index));
    index = nextIndex + 1;
  }

  // We skip the 5 first entries as they may not be possible to obtain
  // in time according to the starting position.
  int j = 0; // step of the kick
  for (int i = 0; i < 5 && getline(motion, line); ++i)
    ++j;

  // We then initialize the angles and timelist arrays.
  mAngleLists.arraySetSize(n);
  mTimeLists.arraySetSize(n);

  index = 0;
  double value = 0.0;
  string* lastAngleTerm = new string[n];
  string timeTerm;

  if (getline(motion, line) && line.compare("")) {
    for (int i = 0; i < n + 2; ++i) {
      nextIndex = line.find(",", index);
      if (i == 0) { // time value
        timeTerm = line.substr(index, nextIndex - index);
        timeTerm.erase (remove(timeTerm.begin(), timeTerm.end(), ':'), timeTerm.end());
        value = atof(timeTerm.c_str()) / 1000.0; // works since the motion lasts < 1 min

        for (int k = 0; k < n; ++k) {
          mTimeLists[k] = ALValue::array(value);
        }
      } else if (i >= 2) { // angle value
        string term = line.substr(index, nextIndex - index);
        lastAngleTerm[i - 2] = term;
        mAngleLists[i - 2] = ALValue::array(atof(term.c_str()));
      }
      // the value i == 1 corresponds to a name like "PoseX"

      index = nextIndex + 1;
    }
    ++j;
  }

  // and we finally add the following elements to the arrays
  while (getline(motion, line) && line.compare("")) {
    for (int i = 0; i < n + 2; ++i) {
      nextIndex = line.find(",", index);
      if (i == 0) {
        timeTerm = line.substr(index, nextIndex - index);
        timeTerm.erase (remove(timeTerm.begin(), timeTerm.end(), ':'), timeTerm.end());
        value = atof(timeTerm.c_str()) / 1000.0;
      } else if (i >= 2) {
        string term = line.substr(index, nextIndex - index);
        if (term.compare(lastAngleTerm[i - 2])) {
          lastAngleTerm[i - 2] = term;
          mTimeLists[i - 2].arrayPush(value);
          mAngleLists[i - 2].arrayPush(atof(term.c_str()));
        }
      }

      index = nextIndex + 1;
    }
    ++j;

    // The motion described is too fast for naoqi, we drop one out of two
    // steps (except the steps 73 to 92, which are the kick itself) so as
    // to let naoqi interpolate the angles itself. Otherwise, Naoqi complains
    // that the moves are faster than the max body velocity and crashes.
    if (!((j > 73) && (j < 92)))
      getline(motion, line);
  }
}

bool Player::isBlue() const {
  return mTeamID == TEAM_BLUE;
}

bool Player::isRed() const {
  return mTeamID == TEAM_RED;
}

int Player::accept_client(int server_fd) {
  int cfd;
  struct sockaddr_in client;
#ifndef _WIN32
  socklen_t asize;
#else
  int asize;
#endif

  asize = sizeof(struct sockaddr_in);

  cfd = accept(server_fd, (struct sockaddr *) &client, &asize);

  if (cfd == -1) {
      cerr << "(" << mPlayerID << "," << mTeamID << ") cannot accept client" << endl;
      return -1;
  }

  return cfd;
}

int Player::create_socket_server(int port) {
  int sfd, rc;
  struct sockaddr_in address;

#ifdef _WIN32
  /* initialize the socket api */
  WSADATA info;

  rc = WSAStartup(MAKEWORD(1, 1), &info); /* Winsock 1.1 */
  if (rc != 0) {
      cerr << "(" << mPlayerID << "," << mTeamID << ") cannot initialize Winsock" << endl;
      return -1;
  }
#endif

  /* create the socket */
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
      cerr << "(" << mPlayerID << "," << mTeamID << ") cannot create socket" << endl;
      return -1;
  }

  /* fill in socket address */
  memset(&address, 0, sizeof(struct sockaddr_in));
  address.sin_family = AF_INET;
  address.sin_port = htons((unsigned short) port);
  address.sin_addr.s_addr = INADDR_ANY;

  /* bind to port */
  rc = ::bind(sfd, (struct sockaddr *) &address, sizeof(struct sockaddr));
  if (rc == -1) {
      cerr << "(" << mPlayerID << "," << mTeamID << ") cannot bind port" << port << endl;
      return -1;
  }

  /* listen for connections */
  if (listen(sfd, MAX_NUM_CONNECTIONS) == -1) {
      cerr << "(" << mPlayerID << "," << mTeamID << ") cannot listen for connections" << endl;
      return -1;
  }

  return accept_client(sfd);
}
