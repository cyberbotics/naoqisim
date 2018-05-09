#ifndef VISION_MANAGER_HPP
#define VISION_MANAGER_HPP

/**
 * Description: Class containing the functions for recognizing the elements
 *              on the soccer field.
 *
 *              When returning (x,y)-coordinates from an image, they are based on
 *              the following model:
 *
 *              Top Left Corner [ 1  1]   [ 0  1]   [-1  1]
 *                              [ 1  0]   [ 0  0]   [-1  0]
 *                              [ 1 -1]   [ 0 -1]   [-1 -1] Bottom Right Corner
 *
 *              You may have to adapt these coordinates once returned to be
 *              consistent with your robot.
 */

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class VisionManager {
public:
  VisionManager(int width, int height, bool alpha);
  ~VisionManager();
  bool getBallCenter(bool &lock, double &x, double &y, const unsigned char *image);
  bool getGoalPosition(bool &wrongSide, double &p1, double &p2, const unsigned char *image);
  bool getRFootPosition(double &pos, const unsigned char *image);

private:
  bool mAlpha; // true if the images that it will receive contain an alpha channel
  int mWidth;
  int mMiddleX;
  int mHeight;
  int mMiddleY;

  double getXCoordinate (int value);
};

#endif
