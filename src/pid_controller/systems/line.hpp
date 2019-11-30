#ifndef CM_PID_LINE_H
#define CM_PID_LINE_H

#include "regulator_data.hpp"

#define MAX_INPUT_ANGLE 90
#define MAX_INPUT_SPEED 128

/**
  * Line follower
  * This control systems regulates the input speed and angle to make sure
  * that the car goes as straight as possible within the track limits
  */
pid_system_out pid_line(pid_system_out &in);

#endif
