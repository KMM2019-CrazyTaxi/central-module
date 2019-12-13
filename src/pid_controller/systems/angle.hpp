#ifndef CM_PID_LINE_H
#define CM_PID_LINE_H

#include "regulator_data.hpp"


/**
  * This system makes sure that the car stays within bounds while the car is following lines.
  */
pid_system_out pid_angle(pid_decision_data &in);

#endif
