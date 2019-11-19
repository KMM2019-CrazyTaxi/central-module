#ifndef CM_REGISTRY_ENTRIES_H
#define CM_REGISTRY_ENTRIES_H

#include <cstdint>


#define SENSOR_DATA_ID "sensor_data"
#define CONTROL_CHANGE_DATA_ID "control_change_data"
#define REGULATOR_DATA_ID "regulator_data"
#define REGULATOR_OUTPUT_ID "regulator_output"

struct sensor_data {
    float distance;
    float acceleration;
};

struct control_change_data {
    char speed_delta;
    char angle_delta;
};

/**
 * Control parameters which the regulator uses to
 * determine how much weight to put on certain parts of the
 * system.
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
struct regulator_parameters{
  float k;
  float td;
};

struct regulator_data {
  // *** Speed metrics ***
  regulator_parameters speed_params;
  uint8_t curr_speed;
  uint8_t input_speed;
  
  // *** Turn metrics ***
  regulator_parameters turn_params;
  uint8_t curr_turn;
  uint8_t input_turn;
  uint8_t dist_left;
  uint8_t dist_right;
};

struct regulator_output {
  uint8_t speed;
  uint8_t turn;
};

#endif
