# naoqisim
NAOqi enabled controller for simulated NAO robots in Webots

## DISCLAIMER

This software is provided as-is with the hope it may help users. It may or may not work, without any guarantee. It is not maintained any more, neither by Aldebaran / SoftBank Robotics, nor by Cyberbotics.

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

### Short version

Start Webots, open the `worlds/nao.wbt` world file and run the simulation in real-time mode.
Start Choregraphe.
Choose Connection > Connect to or click the "Connect to" button.
Click the "Wake Up" button (sun-like icon on the top right corner of the Choregraphe window) to make sure the stiffness is on.

For more information, refer to: http://doc.aldebaran.com/2-1/software/webots/webots_index.html

### Using Webots with Choregraphe

These instructions have been tested with Webots R2018a and Choregraphe 2.1.4.
Please note that Webots must not be launched as root when using any world containing naoqisim, otherwise Choregraphe won't be able to send instructions to the robot in Webots.

Start Webots and open this world file: "naoqisim/worlds/nao.wbt" You should see a red Nao in an empty environment.
If the simulation is paused, then please start it by pushing the `Real-time` button in Webots.

The camera images in Webots (small purple viewports) should reflect what the robot sees.

Several lines of text information corresponding to the output of NAOqi should be printed to Webots console.
Also, a couple of harmless error messages may be displayed, you can safely ignore them:
```
[naoqisim] Qt: Untested Windows version 6.2 detected!
[naoqisim] Error: %1 is not a valid Win32 application.
[naoqisim]  (dynamic library)
[naoqisim] Error: failed to load > C:/Program Files/Webots/resources/projects/plugins/robot_windows/generic/generic.dll library
```

Now you can start Choregraphe with the --no-naoqi option.
Please make sure the Choregraphe version matches the NAOqi version printed in Webots console.
In Choregraphe choose the menu `Connection / Connect to...`.
Then, in the list, select the NAOqi that was started by Webots, on you local machine, it will have the port number 9559, unless you change it.
Note that the NAOqi will not appear in the list if the simulation was not started in Webots.
If the simulation was started but the robot still doesn't appear in the list, force the IP and port to 127.0.0.1 and 9559 in Choregraphe and then press connect.

At this point a Nao model matching the Webots model should appear in Choregraphe.
Now, in Choregraphe toggle the "Wake up" button, which is a little sun in the top right of the window.
Nao is currently in the "Stand Zero" pose, you can change its starting pose using the posture library in Choregraphe.

Then, double-click on any of the Nao parts in Choregraphe: a small window with control sliders appears.
Now, move any of the sliders: the motor movement in Choregraphe should be reflected in the Webots simulation.
If you open the Video monitor in Choregraphe you should see the picture of the Nao camera simulated by Webots.

It is possible to have several Nao robots in your simulation.
However, each Nao robot must use a different NAOqi port.
This can be done in the `controllerArgs` field in the newly created robot, e.g. 9560.

#### Using Motion Boxes

Now we can test some of the motion boxes of Choregraphe.
A simple example is a sit down -> stand up motion.
In Choregraphe, select the "Sit Down" and "Stand Up" boxes from `Box libraries > default`.
Drag and drop them in central view.
Then connect the global "onStart" input to the "Sit Down" box's "onStart" input, and the output of this box to the "Stand Up" box's "onStart" input.
Now, make sure the simulation is running, and push the `Play` button in Choregraphe.
This will make the robot sit down, and then stand up once he is done sitting down.

#### Using the Cameras

Webots simulates Nao's top and bottom cameras.
Using SoftBank Robotics's Choregraphe or the Monitor programs, it is possible to switch between these cameras.
In Choregraphe, use the "Select Camera" box in `Box Library / Vision`.
The simulated camera image can be viewed in Choregraphe: `View / Video monitor`.
The resolution of the image capture can be changed in Webots using the `cameraWidth` and `cameraHeight` fields of the robot.
Note that the simulation speed decreases as the resolution increases.
It is possible to hide the camera viewports (purple frame) in Webots, by setting the `cameraPixelSize` field to 0.
It is also possible to completely switch off the camera simulation by adding the "-nocam" option before the NAOqi port number in the `controllerArgs` field, e.g. "-nocam 9559".

