# naoqisim
naoqisim controller for simulated NAO robots in Webots

## Introduction

This repository contains the source code and dependencies needed to build the naoqisim Webots controller. This controller is used to connect a Webots-simulated NAO robot to the NAOqi programming interface, including the Choregraphe graphical programming interface.

## Dependencies

### Development environment
On Windows, it is necessary to install Microsoft Visual Studio C++ 2010 Express. You should also install MSYS2 from https://www.msys2.org to have a convienient UNIX-like environment to be able to execute the Makefiles.
On Linux, you will need the standard gcc compiler suite.
On recent macOS distributions, naoqisim doesn't work any more.

### Simulation SDK
These libraries were provided by Aldebaran Robotics Windows, Linux and Mac. However they are not maintained anymore and may not work on recent systems, including the latest versions of macOS. You can install them by typing `make` in the `aldebaran` directory.

### Choregraphe
The Choregraphe suite software should be downloaded from SoftBank Robotics (formerly Aldebaran Robotics).
Version 2.1.4.13 was recently tested and is known to work with naoqisim.

### Webots
Webots can be downloaded from https://cyberbotics.com
Version R2018a was recently tested and is known to work with naoqisim.

## Build

Set the `WEBOTS_HOME` environment variable to point to the Webots installation folder, as documented in the Webots user guide.

### Windows
Open `controllers/naoqisim/naoqisim.sln` with Visual C++ and build the project.

### Linux
Type `make` in the `controllers/naoqisim` folder.

## Use

Start Webots, open the `worlds/nao.wbt` world file and run the simulation in real-time mode.
Start Choregraphe.
Choose Connection > Connect to or click the "Connect to" button.
Click the "Wake Up" button (sun-like icon on the top right corner of the Choregraphe window) to make sure the stiffness is on.

For more information, refer to: http://doc.aldebaran.com/2-1/software/webots/webots_index.html
