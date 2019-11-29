#ifndef CM_REGISTRY_ENTRIES_H
#define CM_REGISTRY_ENTRIES_H

#include <cstdint>

#define SENSOR_DATA_ID 				    1
#define CONTROL_CHANGE_DATA_ID 		2
#define TELEMETRICS_DATA_ID 		  3
#define REGULATOR_OUT_DATA_ID 		4
#define REGULATOR_PARAM_DATA_ID 	5
#define REGULATOR_SAMPLE_DATA_ID 	6
#define GRAPH_ID                  7

struct sensor_data {
    int16_t acc_x;
    int16_t acc_y;
    int16_t acc_z;
    uint8_t dist;
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
  double kp = 5;
  double ki;
  double kd = 5;
  double alpha;
  double beta;

  double angle_threshold;
  double speed_threshold;
  double min_value;
  double slope = 1;
};

/**
  * Contains all of the parameters for the whole control system
  */
struct regulator_param_data{
  pid_params turning;
  pid_params parking;
  pid_params stopping;
  pid_params line_angle;
  pid_params line_speed;
};

/**
 * Contains sample data for different pid sub-systems
 */
struct regulator_sample_data{
  double line_angle_d;
  double line_speed_d;
  double stopping_speed_d;
};

#endif
