#ifndef CM_PID_STOPPING_H
#define CM_PID_STOPPING_H

#include "regulator_data.hpp"

/**
  * This system is used when the car is getting to a full stop
  */
pid_system_out pid_stopping(const pid_decision_data &in);

#endif
