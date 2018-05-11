#ifndef CONST_HPP
#define CONST_HPP

// Nao cameras
#define TOP_CAM 0
#define BOT_CAM 1

// There is currently an issue with the angles, the values
// specified through the NAOqi API depend on the simulation speed
// The following are for a simulation speed of 1. If this speed
// cannot be reached, the behavior should still work, but in
// case it does not, these values must be raised.
#define ANGLE_BOT 0.4
#define ANGLE_ITER 0.2

// Colors
#define NONE 0
#define WHITE 1
#define BLACK 2
#define RED 3
#define BLUE 4
#define LIGHT_GREEN 5
#define YELLOW 6
#define DARK_BROWN 7

#endif
