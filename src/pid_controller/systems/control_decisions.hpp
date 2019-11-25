#ifndef CM_PID_DECISIONS_H
#define CM_PID_DECISIONS_H

#include "regulator_data.hpp"
#include "turning.hpp"
#include "parking.hpp"
#include "stopping.hpp"
#include "line.hpp"

/**
  * This system makes control decisions and sends it forward to the next
  * appropriate sub-system.
  */
pid_decision_return pid_decision(const pid_decision_in &in);

#endif
