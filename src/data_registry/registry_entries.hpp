#ifndef CM_REGISTRY_ENTRIES_H
#define CM_REGISTRY_ENTRIES_H

#include <cstdint>


#define SENSOR_DATA_ID "sensor_data"
#define CONTROL_CHANGE_DATA_ID "control_change_data"
#define TELEMETRICS_DATA_ID "telemetrics_data"
#define REGULATOR_OUT_DATA_ID "regulator_out_data"
#define REGULATOR_PARAM_DATA_ID "regulator_param_data"
#define REGULATOR_SAMPLE_DATA_ID "regulator_sample_data"

struct sensor_data {
    float distance;
    float acceleration;
};

struct control_change_data {
    char speed_delta;
    char angle_delta;
};

struct telemetrics_data {
  double curr_speed;
  double dist_left;
  double dist_right;
};

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
 * Increasing k leads to:
 * - Increased speed
 * - Decreased stability marginals
 * - Improved compensation of process disturbances
 * - Increased control signal activity
 *
 * Increasing td leads to:
 * - Better stability marginals (larger Td-value gives
 *   better stability)
 * - Increased impact from measurement error
 */
struct pid_params{
  double kp;
  double ki;
  double kd;
  double alpha;
  double beta;
};

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
};

#endif
