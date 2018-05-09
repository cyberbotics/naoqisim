#include "Image.hpp"

Image::Image(int width, int height, const unsigned char *data) {
  mWidth = width;
  mHeight = height;
  mData = (unsigned char *)data;
  mFreeData = false;
}

Image::~Image() {
  if (mFreeData)
    delete [] mData;
}

// Convert the image from BGRA to RGB and perform a basic 'pixel' enlarging resample.
// Note: it takes about 1ms to convert a 160x120 image in my environment
void Image::convert(int newWidth, int newHeight) {
  // create new buffer to interpolate into
  unsigned char *newData = new unsigned char [newWidth * newHeight * 3];

  double scaleHeight = (double)newHeight / (double)mHeight;
  double oneOverScaleHeight = 1.0 / scaleHeight;
  int threeNewWidth = 3 * newWidth;
  int fourWidth = 4 * mWidth;

  for (int cy = 0; cy < newHeight; cy++) {
    int lineIndex = cy * threeNewWidth;
    int nearestLine = ((int)(cy * oneOverScaleHeight) * fourWidth);
    for (int cx = 0; cx < newWidth; cx++) {
      int pixelIndex = lineIndex + 3 * cx;
      int nearestMatch = nearestLine + ((int)(cx * oneOverScaleHeight) * 4);
      newData[pixelIndex++] =  mData[nearestMatch + 2];
      newData[pixelIndex++] =  mData[nearestMatch + 1];
      newData[pixelIndex  ] =  mData[nearestMatch    ];
    }
  }

  if (mFreeData)
    delete [] mData;

  mData = newData;
  mFreeData = true;
  mWidth = newWidth;
  mHeight = newHeight;
}
