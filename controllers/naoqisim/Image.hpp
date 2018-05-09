#ifndef IMAGE_HPP
#define IMAGE_HPP

// Description: Class for storing and resampling pixmap images

class Image {
public:
  // create/destroy image
  Image(int width, int height, const unsigned char *data); // data is a pointer to a BGRA image buffer
  virtual ~Image();

  // RGB image data in bytes as an array of [width][height][3]
  unsigned char *data() const { return mData; }

  // // change image format from BGRA to RGB and dimensions to the ones specified
  void convert(int newWidth, int newHeight);

private:
  int mWidth, mHeight;
  unsigned char *mData;
  bool mFreeData;
};

#endif
