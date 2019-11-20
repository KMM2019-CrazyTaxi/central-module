#ifndef CM_PID_DECISIONS_H
#define CM_PID_DECISIONS_H

#include "regulator_data.hpp"
#include "turning.hpp"
#include "parking.hpp"
#include "stopping.hpp"
#include "line.hpp"

regulator_out_data pid_decision(const pid_decision_in &in);

#endif
