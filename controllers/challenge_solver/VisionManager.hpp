#ifndef VISION_MANAGER_HPP
#define VISION_MANAGER_HPP

/**
 * Description: Contains the functions processing what Nao is seeing.
 */

#include <alerror/alerror.h>
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alvisiondefinitions.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define MAX_IMG_TRY 3

class VisionManager {
public:
  VisionManager(boost::shared_ptr<AL::ALBroker> broker);
  ~VisionManager();
  bool seeColor();
  bool seeColor(int filter, int camera);
  bool seeEnoughColor(double proportion);
  bool seeEnoughColor(int filter, int camera, double proportion);
  bool hasKeyInHand();
  double findOrientation();

private:
  int mCameraIndex;
  int mColorSpace;
  int mResolution;
  int mFps;
  int mWidth;
  int mHeight;
  int mFilter;
  std::string mNameBot;
  std::string mNameTop;
  cv::Mat mFiltered;
  boost::shared_ptr<AL::ALVideoDeviceProxy> mBotCameraProxy;
  boost::shared_ptr<AL::ALVideoDeviceProxy> mTopCameraProxy;

  void refreshImage();
  void switchCamera();
  void applyFilter(int filter);
};
#endif
