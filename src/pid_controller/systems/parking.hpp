#ifndef CM_PID_PARKING_H
#define CM_PID_PARKING_H

#include "regulator_data.hpp"

/**
  * This system is used while parking on the side of the track
  */
pid_system_out pid_parking(const pid_decision_data &in);

#endif
