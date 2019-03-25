# naoqisim
NAOqi enabled controller for simulated NAO robots in Webots

## DISCLAIMER

This software is provided as-is with the hope it may help users. It may or may not work, without any guarantee. It is not maintained any more, neither by Aldebaran / SoftBank Robotics, nor by Cyberbotics.

## Introduction

This repository contains the source code and dependencies needed to build the naoqisim Webots controller. This controller is used to connect a Webots-simulated NAO robot to the NAOqi programming interface, including the Choregraphe graphical programming interface.

## Dependencies

### Development environment
On Windows, it is necessary to install Microsoft Visual Studio C++ 2010 Express. You should also install MSYS2 (64 bit version) from https://www.msys2.org to have a convienient UNIX-like environment to be able to execute the Makefiles.
On Linux, you will need the standard gcc compiler suite.
On recent macOS distributions, naoqisim doesn't work any more.

### Simulation SDK
These libraries were provided by Aldebaran Robotics Windows, Linux and Mac. However they are not maintained anymore and may not work on recent systems, including the latest versions of macOS. You can install them by typing `make` in the `aldebaran` directory.

### Choregraphe
The Choregraphe suite software should be downloaded from SoftBank Robotics (formerly Aldebaran Robotics).
Version 2.1.4.13 was recently tested and is known to work with naoqisim.

### Webots
Webots can be downloaded from https://cyberbotics.com
You should use the latest version of Webots.
Version R2019a revision 1 was recently tested and is known to work with naoqisim, but later versions should work as well.

## Build

Set the `WEBOTS_HOME` environment variable to point to the Webots installation folder, as documented in the Webots user guide.

### Windows
Open the MSYS2 console, `cd` to the naoqisim root directory. Type `make` to complete the installation of the Simulator SDK and the naoqisim controller. On the Simulator SDK is installed, you can compile the naoqisim controller by opening `controllers/naoqisim/naoqisim.sln` with Visual C++ to build the project.

### Linux
Add `WEBOTS_HOME/lib` to your library path (e.g. `export LD_LIBRARY_PATH=$WEBOTS_HOME/lib`).
Type `make` in the naoqisim root directory to install the Simulation SDK and naoqisim controller. Once the Simulation SDK is installed, you can compile the naoqisim controller by typing `make` in the `controllers/naoqisim` folder.

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

### Known Problems

#### macOS Support

SoftBank Robotics dropped the `simulator SDK` support for macOS since the `2.1.2.17` version.
Webots includes this latest version for macOS, however it doesn't work on recent macOS versions.

#### Timing Issues: Getting the Right Speed for Realistic Simulation

Choregraphe uses exclusively real-time and so the robot's motions are meant to be carried out in real-time.
The Webots simulator uses a virtual time base that can be faster or slower than real-time, depending on the CPU and GPU power of the host computer.
If the CPU and GPU are powerful enough, Webots can keep up with real-time, in this case the speed indicator in Webots shows approximately 1.0x, otherwise the speed indicator goes below 1.0x.
Choregraphe motions will play accurately only if Webots simulation speed is around 1.0x.
When Webots simulation speed drifts away from 1.0x, the physics simulation becomes wrong (unnatural) and thus Choregraphe motions don't work as expected anymore.
For example, if Webots indicates 0.5x, this means that it is only able to simulate at half real-time the motion provided by Choregraphe: the physics simulation is too slow.
Therefore it is important to keep the simulation speed as close as possible to 1.0x.
There are currently no means of synchronizing Webots and Choregraphe, but this problem will be addressed in a future release.
It is often possible to prevent the simulation speed from going below 1.0x, by keeping the CPU and GPU load as low as possible.
There are several ways to do that, here are the most effective ones:

- Switch off the simulation of the Nao cameras with the "-nocam" option, as mentioned above.
- Increase the value of `WorldInfo.displayRefesh` in the Scene Tree.
- Switch off the rendering of the shadows: change to FALSE the `castShadows` field of each light source in the Scene Tree.
- Reduce the dimensions of the 3D view in Webots, by manually resizing the GUI components.
- Remove unnecessary objects from the simulation, in particular objects with physics.

#### Unexpected Webots Crashes

If for some unexpected reason Webots crashes, it is possible that the `hal` or `naoqi-bin` processes remain active in memory.
In this case we recommend you to terminate these processes manually before restarting Webots.

On Windows, use the Task Manager (the Task Manager can be started by pressing Ctrl+Alt+Delete): In the Task Manager select the `Processes` tab, then select each `hal.exe` and `naoqi-bin.exe` line and push the "End Process" button for each one.

On Linux, you can use the `killall` or the `pkill` commands, e.g.:

```sh
$ killall hal naoqi-bin
```
