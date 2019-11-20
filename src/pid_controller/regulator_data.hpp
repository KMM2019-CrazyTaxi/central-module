/**
 * regulator_data.hpp
 * This file contains internal data structures for the
 * PID-regulator system.
 *
 * NOTE: This file does NOT contain definitions of
 *       global data for the PID-regulator system!
 */

#ifndef CM_PID_REGULATOR_DATA_H
#define CM_PID_REGULATOR_DATA_H

#include <cstdint>
#include "registry_entries.hpp"

enum System {decision, turning, parking, stopping, line};
enum Turn {left, right};

/**
 * In to the control decision system
 */
struct pid_decision_in{
  telemetrics_data metrics;
  regulator_param_data params;
};

struct pid_decision_data{
  telemetrics_data metrics;
  regulator_param_data params;
  System sys;
  Turn turn;
};

struct pid_turning_in{
  telemetrics_data metrics;
  pid_params params;
  Turn turn;
};

struct pid_turning_out{
  uint8_t angle;
  uint8_t speed;
};

struct pid_parking_in{
  telemetrics_data metrics;
  pid_params params;
};

struct pid_parking_out{
  uint8_t angle;
  uint8_t speed;
};

struct pid_stopping_in{
  telemetrics_data metrics;
  pid_params params;
};

struct pid_stopping_out{
  uint8_t angle;
  uint8_t speed;
};

struct pid_line_in{
  uint8_t angle;
  uint8_t speed;
  pid_params params;
};

struct pid_line_out{
  uint8_t angle;
  uint8_t speed;
};

#endif
