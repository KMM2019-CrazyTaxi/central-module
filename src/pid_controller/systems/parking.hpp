#ifndef CM_PID_PARKING_H
#define CM_PID_PARKING_H

#include "regulator_data.hpp"

pid_system_out pid_parking(const pid_decision_data &in);

#endif
