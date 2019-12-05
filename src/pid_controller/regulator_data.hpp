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
#include "pathfinding.hpp"
#include "graph.hpp"

#define MAX_INPUT_ANGLE 90
#define MAX_INPUT_SPEED 128

/**
 * Enumerates all of the available sub systems.
 */
enum System {decision, turning, parking, stopping, line};

/**
  * Map data
  */
struct map_data{
    int current_pos;
    graph g;
    std::vector<path_step> path;
};

/**
 * Contains data which is sent to the regulator's decision maker.
 */
struct pid_decision_in{
  telemetrics_data metrics;
  sensor_data sensor_data;
  regulator_param_data params;
  map_data map;
  double dt;
  regulator_sample_data samples;
};

/**
 * Contains a sub-system's output data.
 */
struct pid_system_out{
  double angle;
  double speed;
  telemetrics_data metrics;
  sensor_data sensor_data;
  regulator_param_data params;

  double dt;
  regulator_sample_data samples;
};

struct pid_decision_return{
  double angle;
  double speed;
  regulator_sample_data samples;
  int current_pos;
};

/**
 * Contains all output from the regulator's decision maker.
 */
struct pid_decision_data{
  pid_system_out out;
  double dist;
  System sys;
  map_data map;
};

#endif
