#include "VisionManager.hpp"

using namespace cv;
using namespace std;

VisionManager::VisionManager(int width, int height, bool alpha):
  mAlpha(alpha), mWidth(width), mMiddleX(width / 2), mHeight(height), mMiddleY(height / 2) {
}

VisionManager::~VisionManager() {
}

// lock becomes true if the ball is very far
bool VisionManager::getBallCenter(bool &lock, double &x, double &y, const unsigned char *image) {
  int cnt = 0;

  Mat img;
  if (mAlpha)
    img = Mat(Size(mWidth, mHeight), CV_8UC4);
  else
    img = Mat(Size(mWidth, mHeight), CV_8UC3);
  Mat hsv = Mat(Size(mWidth, mHeight), CV_8UC3);
  Mat filtered = Mat(Size(mWidth, mHeight), CV_8UC1);

  x = 0.0;
  y = 0.0;

  img.data = (uchar*) image;
  cvtColor(img,hsv,CV_BGR2HSV);
  inRange(hsv, Scalar(9, 127, 115), Scalar(21, 255, 255), filtered);

  // we ignore border pixels because they may contain some artifacts
  for (int i = 2; i < mHeight - 2; ++i) {
    for (int j = 2; j < mWidth - 2; ++j) {
      if (filtered.at<uchar>(i, j) == 255) {
        x += mMiddleX - j - 0.5;
        y += mMiddleY - i - 0.5;
        ++cnt;
      }
    }
  }

  if (cnt) {
    if (cnt < 0.0002 * mWidth * mHeight)
      lock = true;
    x /= cnt * mMiddleX;
    y /= cnt * mMiddleY;
    return true;
  }

  return false;
}

// p1 and p2 are both an x-coordinate
bool VisionManager::getGoalPosition(bool &wrongSide, double &p1, double &p2, const unsigned char *image) {
  int referencePoint = -1;
  int min1 = mWidth;
  int max1 = -1;
  int min2 = mWidth;
  int max2 = -1;
  int maxHeight = 0;
  int lMargin = 0;
  int uMargin = 0;
  int offsetPost = mWidth / 15;
  int offsetBetweenPosts = mWidth / 5;
  Mat img;
  if (mAlpha)
    img = Mat(Size(mWidth, mHeight), CV_8UC4);
  else
    img = Mat(Size(mWidth, mHeight), CV_8UC3);
  Mat hsv = Mat(Size(mWidth, mHeight), CV_8UC3);
  Mat filtered = Mat(Size(mWidth, mHeight), CV_8UC1);
  Mat edges;

  wrongSide = false;
  img.data = (uchar*) image;
  cvtColor(img,hsv,CV_BGR2HSV);
  inRange(hsv, Scalar(24, 127, 115), Scalar(26, 255, 255), filtered);

  // The previous color filter can detect pixels of the ball too, so
  // we have to do some checks about the shape too.
  for (int i = 0; i < mHeight; ++i) {
    lMargin = i - offsetPost < 0 ? 0 : i - offsetPost;
    uMargin = i + offsetPost > mHeight - 1 ? mHeight - 1 : i + offsetPost;
    for (int j = 0; j < mWidth; ++j) {
      if ((filtered.at<uchar>(i, j) == 255) &&
          (filtered.at<uchar>(lMargin, j) == 255) &&
          (filtered.at<uchar>(uMargin, j) == 255)) {
        // Point of reference to distinguate between the two posts
        if (referencePoint < 0)
          referencePoint = j;
        if (abs(referencePoint - j) < offsetBetweenPosts) {
          if (j > max1)
            max1 = j;
          if (j < min1)
            min1 = j;
        } else {
          if (j > max2)
            max2 = j;
          if (j < min2)
            min2 = j;
        }
        if (i > maxHeight)
          maxHeight = i;
      }
    }
  }

  // Case 1: we see the two posts.
  if (max2 > 0) {
    if (max1 < min2) {
      p1 = getXCoordinate(max1);
      p2 = getXCoordinate(min2);
    } else {
      p1 = getXCoordinate(max2);
      p2 = getXCoordinate(min1);
    }

    return true;
  }

  // Case 2: we see only one post.
  if (max1 >= 0) {
    // Look in which direction the horizontal post goes
    for (int j = 0; j < mWidth; ++j) {
      lMargin = j - offsetPost < 0 ? 0 : j - offsetPost;
      uMargin = j + offsetPost > mWidth - 1 ? mWidth - 1 : j + offsetPost;
      for (int i = 0; i < mHeight; ++i) {
        if ((filtered.at<uchar>(i, lMargin) == 255) &&
            (filtered.at<uchar>(i, uMargin) == 255)) {
          if (j < min1) {
            p1 = getXCoordinate(0);
            p2 = getXCoordinate(min1);
            return true;
          } else if (j > max1) {
            p1 = getXCoordinate(max1);
            p2 = getXCoordinate(mWidth - 1);
            return true;
          }
        }
      }
    }

    // If we see only one post but don't see the horizontal post, we can
    // check for the net to know which post it is.
    if (countNonZero(filtered) > (0.008 * mWidth * mHeight)) {
      int minByColumn = maxHeight / 10;
      int minLeft = mWidth;
      int maxRight = -1;
      img.data = (uchar*) image;
      cvtColor(img,hsv,CV_BGR2HSV);
      inRange(hsv, Scalar(0,0,240), Scalar(0,0,255), filtered);

      int cntL = 0;
      int cntR = 0;

      for (int j = 0; j < max1; ++j) {
        int cnt = 0;
        for (int i = 0; i < maxHeight; ++i) {
          if (filtered.at<uchar>(i, j) == 255) {
            ++cntL;
            ++cnt;
            if ((minLeft > j) && (cnt == minByColumn))
              minLeft = j;
          }
        }
      }

      for (int j = max1; j < mWidth; ++j) {
        int cnt = 0;
        for (int i = 0; i < maxHeight; ++i) {
          if (filtered.at<uchar>(i, j) == 255) {
            ++cntR;
            ++cnt;
            if ((maxRight < j) && (cnt == minByColumn))
              maxRight = j;
          }
        }
      }

      int minMargin = 0.0005 * mWidth * mHeight;
      if (cntL < minMargin && cntR < minMargin)
        return false;

      if (cntL > cntR) {
        p1 = getXCoordinate(minLeft);
        p2 = getXCoordinate(min1);
        if (minLeft > (mWidth / 25)) {
          p1 = -1;
          wrongSide = true;
          return false;
        }
      } else {
        p1 = getXCoordinate(max1);
        p2 = getXCoordinate(maxRight);
        if (maxRight < ((24 * mWidth) / 25)) {
          p2 = 1;
          wrongSide = true;
          return false;
        }
      }

      return true;
    }
  }

  return false;
}

// Warning: only compatible with ROBOTIS OP2. You can also infer the coordinate of the foot
// without the vision as it shouldn't vary too much according to the head in most biped robots.
// Note that the function can easily be adaptable to other robots by simply adding an argument
// defining the robot's foot color, as long as it is not the same color as an element of the field.
bool VisionManager::getRFootPosition(double &pos, const unsigned char *image) {
  Mat img;
  if (mAlpha)
    img = Mat(Size(mWidth, mHeight), CV_8UC4);
  else
    img = Mat(Size(mWidth, mHeight), CV_8UC3);
  Mat hsv = Mat(Size(mWidth, mHeight), CV_8UC3);
  Mat filtered = Mat(Size(mWidth, mHeight), CV_8UC1);

  img.data = (uchar*) image;
  cvtColor(img,hsv,CV_BGR2HSV);
  inRange(hsv, Scalar(0, 0, 30), Scalar(0, 0, 155), filtered);

  bool seen = false;
  int left = 0;
  int right = 0;
  int j;

  // Find the space between the two feet
  for (j = (3 * mWidth) / 10; j < mWidth; ++j) {
    seen = false;
    // Try to avoid seeing the foots of other robots...
    for (int i = (7.5 * mHeight) / 10; i < mHeight; ++i) {
      if (filtered.at<uchar>(i, j) == 255) {
        seen = true;
        break;
      }
    }
    if (!seen)
      break;
  }

  // The next item found will be the right foot
  for (; j < mWidth; ++j) {
    bool justSeen = false;
    for (int i = (7.5 * mHeight) / 10; i < mHeight; ++i) {
      if (filtered.at<uchar>(i, j) == 255) {
        justSeen = true;
        break;
      }
    }
    if (seen && !justSeen) {
      right = j - 1;
      break;
    }
    if (!seen && justSeen) {
      seen = true;
      left = j;
    }
  }

  if (right > left) {
    pos = getXCoordinate((right + left) / 2);
    return true;
  }

  return false;
}

// transforms a pixel coordinate in a [-1, 1] coordinate.
double VisionManager::getXCoordinate (int value) {
  return 1.0 - ((value + 0.5) / mMiddleX);
}
