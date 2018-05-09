#include "Camera.hpp"
#include "Singletons.hpp"
#include "Image.hpp"
#include <webots/robot.h>
#include <webots/camera.h>
#include <iostream>

Camera::Camera(const CameraSensor *sensor, int timeStep) : Device(sensor->name()) {

  mSensor = sensor;

  cout << "simulator-sdk max camera resolution: " << name() << ": " << sensor->width() << "x" << sensor->height() << "\n";

  mTag = wb_robot_get_device(name().c_str());
  if (! mTag) {
    cerr << "Webots Camera not found for Sim::CameraSensor: " << name() << "\n";
    return;
  }

  // enable Webots camera
  wb_camera_enable(mTag, 5*timeStep);

  // width and height of Webots camera images
  mWidth = wb_camera_get_width(mTag);
  mHeight = wb_camera_get_height(mTag);
  cout << "Webots camera resolution: " << name() << ": " << mWidth << "x" << mHeight << "\n";
}

Camera::~Camera() {
}

void Camera::update() {

  if (! mTag)
    return;

  // find out the width and height expected by the HAL
  int bufferSize = 0;
  int width = 0;
  int height = 0;
  Singletons::hal()->cameraBufferSize(mSensor, &bufferSize, &width, &height);
  if (bufferSize == 0 || width == 0 || height == 0)
    return;

  CameraResolution cameraResolution = Singletons::hal()->cameraResolution(mSensor);
  CameraColorSpace cameraColorSpace = Singletons::hal()->cameraColorSpace(mSensor);
  // create image with Webots camera dimensions
  const unsigned char *imageData = wb_camera_get_image(mTag);
  Image image(mWidth, mHeight, imageData);

  // rescale Webots image to HAL image dimensions change format to RGB
  image.convert(width, height);

  // update HAL
  if (! Singletons::hal()->sendCameraSensorValue(
    mSensor,
    (unsigned char *)image.data(),
    cameraResolution,
    cameraColorSpace))
    cerr << "Sim::HALInterface::sendCameraSensorValue() failed.\n";
}
