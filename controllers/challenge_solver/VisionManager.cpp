#include <iostream>
#include "VisionManager.hpp"
#include "const.hpp"

using namespace std;
using namespace cv;
using namespace AL;

VisionManager::VisionManager(boost::shared_ptr<ALBroker> broker) :
  mCameraIndex(TOP_CAM), mColorSpace(kBGRColorSpace), mNameBot("Challenge_Bot"), mNameTop("Challenge_Top")
{
  mBotCameraProxy = boost::shared_ptr<ALVideoDeviceProxy> (new ALVideoDeviceProxy(broker));
  mTopCameraProxy = boost::shared_ptr<ALVideoDeviceProxy> (new ALVideoDeviceProxy(broker));
  mResolution = mTopCameraProxy->getResolution(mCameraIndex);
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

  mFilter = NONE;
  mFiltered = Mat(Size(mWidth, mHeight), CV_8U);
  mNameBot = mBotCameraProxy->subscribeCamera(mNameBot, BOT_CAM, mResolution, mColorSpace, mFps);
  mNameTop = mTopCameraProxy->subscribeCamera(mNameTop, TOP_CAM, mResolution, mColorSpace, mFps);
}

VisionManager::~VisionManager() {
  mBotCameraProxy->unsubscribe(mNameBot);
  mTopCameraProxy->unsubscribe(mNameTop);
}

bool VisionManager::seeColor() {
  refreshImage();

  return (((double) countNonZero(mFiltered) / (mWidth * mHeight)) > 0.002);
}

bool VisionManager::seeColor(int filter, int camera) {
  if (mCameraIndex != camera)
    switchCamera();
  applyFilter(filter);

  return (((double) countNonZero(mFiltered) / (mWidth * mHeight)) > 0.002);
}

bool VisionManager::seeEnoughColor(double prop) {
  refreshImage();

  return (((double) countNonZero(mFiltered) / (mWidth * mHeight)) >= prop);
}

bool VisionManager::seeEnoughColor(int filter, int camera, double prop) {
  if (mCameraIndex != camera)
    switchCamera();
  applyFilter(filter);

  return (((double) countNonZero(mFiltered) / (mWidth * mHeight)) >= prop);
}

bool VisionManager::hasKeyInHand() {
  if (mCameraIndex != TOP_CAM)
    switchCamera();
  applyFilter(RED);

  int red = 0;
  for (int i = 0; i < mHeight; ++i) {
    for (int j = (31.0 / 32.0) * mWidth; j < mWidth; ++j) {
      if (mFiltered.at<uchar>(i, j) == 255)
        ++red;
    }
  }

  return (red > (((1.0 / 32.0) * mWidth * mHeight) / 48));
}

double VisionManager::findOrientation() {
  int cnt = 0;
  int midHor = mWidth / 2;
  double totHor = 0;

  refreshImage();

  for (int i = 0; i < mHeight; ++i) {
    for (int j = 0; j < mWidth; ++j) {
      if (mFiltered.at<uchar>(i, j) == 255) {
        totHor += midHor - j - 0.5;
        ++cnt;
      }
    }
  }

  if (cnt)
    totHor /= cnt * midHor;

  return totHor;
}

void VisionManager::refreshImage() {
  bool imageOk = false;
  Mat img = Mat(Size(mWidth, mHeight), CV_8UC3);
  ALValue image;
  Scalar lMargin;
  Scalar uMargin;

  for (int i = 0; i < MAX_IMG_TRY && !imageOk; ++i) {
    try {
      if (mCameraIndex)
        image = mBotCameraProxy->getImageRemote(mNameBot);
      else
        image = mTopCameraProxy->getImageRemote(mNameTop);
      imageOk = true;
    } catch (ALError e) {
      cerr << "[" << i + 1 << "/" << MAX_IMG_TRY << "] Couldn't get an image, new attempt..." << endl;
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

  img.data = (uchar*) image[6].GetBinary();
  if (mCameraIndex)
    mBotCameraProxy->releaseImage(mNameBot);
  else
    mTopCameraProxy->releaseImage(mNameTop);

  switch (mFilter) {
    case WHITE: {
      lMargin = Scalar(0,0,202);
      uMargin = Scalar(0,0,255);
      break;
    }
    case BLACK: {
      lMargin = Scalar(0,0,0);
      uMargin = Scalar(100,5,5);
      break;
    }
    case RED: {
      lMargin = Scalar(0,40,60);
      uMargin = Scalar(5,255,255);
      break;
    }
    case BLUE: {
      lMargin = Scalar(100,70,120);
      uMargin = Scalar(110,130,150);
      break;
    }
    case LIGHT_GREEN: {
      lMargin = Scalar(60,150,250);
      uMargin = Scalar(60,155,255);
      break;
    }
    case YELLOW: {
      lMargin = Scalar(22, 220, 220);
      uMargin = Scalar(33, 255, 255);
      break;
    }
    case DARK_BROWN: {
      lMargin = Scalar(15,10,10);
      uMargin = Scalar(15,140,100);
      break;
    }
    default: {
      // If there's no filter, the image is void.
      for (int i = 0; i < mHeight; ++i) {
        for (int j = 0; j < mWidth; ++j) {
          mFiltered.at<uchar>(i, j) = 0;
        }
      }
      return;
    }
  }

  Mat hsv = Mat(Size(mWidth, mHeight), CV_8UC3);
  cvtColor(img,hsv,CV_BGR2HSV);
  inRange(hsv, lMargin, uMargin, mFiltered);
}

void VisionManager::switchCamera() {
  mCameraIndex = (mCameraIndex + 1) % 2;
}

void VisionManager::applyFilter(int filter) {
  mFilter = filter;

  refreshImage();
}
