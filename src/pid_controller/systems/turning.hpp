#ifndef CM_PID_TURNING_H
#define CM_PID_TURNING_H

#include "regulator_data.hpp"

/**
  * This system is used while the car is turning
  */
pid_system_out pid_turning(pid_decision_data &in);

#endif
