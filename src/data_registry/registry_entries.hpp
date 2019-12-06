#ifndef CM_REGISTRY_ENTRIES_H
#define CM_REGISTRY_ENTRIES_H

#include <cstdint>
#include <vector>
#include <deque>
#include <utility>
#include "graph.hpp"

#define SENSOR_DATA_ID              1
#define CONTROL_CHANGE_DATA_ID      2
#define TELEMETRICS_DATA_ID         3
#define REGULATOR_OUT_DATA_ID       4
#define REGULATOR_PARAM_DATA_ID     5
#define REGULATOR_SAMPLE_DATA_ID    6
#define MISSION_DATA_ID             7
#define PATH_ID                     8
#define MODE_ID                     9

struct sensor_data {
    int16_t acc_x;
    int16_t acc_y;
    int16_t acc_z;
    uint16_t dist;
    uint8_t speed;
};

struct control_change_data {
    char speed;
    char angle;
};

/**
  * Necessary data sent to the control system to describe the environment
  */
struct telemetrics_data {
    double curr_speed;
    double dist_left;
    double dist_right;
    double dist_stop_line;
};

/**
  * Output of the whole control system, sent to the control module
  */
struct regulator_out_data {
    double speed;
    double angle;
};

/**
 * Control parameters which the regulator uses to
 * determine how much weight to put on certain parts of the
 * system.
 *
 * NOTE: Not a global type itself, part of regulator_param_data.
 *
 * Increasing kp leads to:
 * - Decreased rise time
 * - Increased overshoot
 * - Small settling time change
 * - Decreased steady-state error
 * - Degraded stability
 *
 * Increasing ki leads to:
 * - Decreased rise time
 * - Increased overshoot
 * - Increased settling time
 * - Eliminated steady-state error
 * - Degraded stability
 *
 * Increasing kd leads to:
 * - Minor rise time change
 * - Decreased overshoot
 * - Decreased settling time
 * - No effect of steady-state error (in theory)
 * - Improved stability if kd is small
 *
 * The rest of the data is system-specific in how they are used.
 */
struct pid_params{
    double kp;
    double ki;
    double kd;
    double alpha;
    double beta;

    double angle_threshold;
    double speed_threshold;
    double min_value;
    double slope;
};

/**
  * Contains all of the parameters for the whole control system
  */
struct regulator_param_data{
  pid_params turning =
  {
      .kp = 1,
      .ki = 0,
      .kd = 0,
      .alpha = 1,
      .beta = 1
  };
  pid_params parking;
  pid_params stopping;
  pid_params line_angle =
  {
      .kp = 10,
      .ki = 0,
      .kd = 0,
      .alpha = 1,
      .beta = 1
  };
  pid_params line_speed =
  {
      .kp = 0,
      .ki = 0,
      .kd = 0,
      .alpha = 1,
      .beta = 1,
      .angle_threshold = 5,
      .speed_threshold = 5,
      .min_value = 0.5,
      .slope = 1
  };
};

/**
 * Contains sample data for different pid sub-systems
 */
struct regulator_sample_data{
  double line_angle_d;
  double line_speed_d;
  double stopping_speed_d;
  double dist_stop_line;
};

/**
  * Contains data about the map and missions
  */
struct mission_data{
    graph g;
    std::deque<std::pair<int,int>> missions;
    int current_pos = 0;
};

/**
 * The control mode of the system
 */
enum mode : int8_t { AUTONOMOUS, MANUAL};

#endif
