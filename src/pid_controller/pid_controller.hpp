#ifndef CM_PID_CONTROLLER_H
#define CM_PID_CONTROLLER_H

#include "defs.hpp"

#define K_SPEED_INITIAL = 1

#define K_TURN_INITIAL = 1
#define TD_TURN_INITIAL = 0

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
struct ctrl_parameters{
  float k;
  float td;
};

/**
 * Output values from the whole PID-controller
 */
struct ctrl_output{
  uint8_t speed;
  uint8_t turn;
};

/**
 * Input metrics to the SPEED-regulator
 */
struct ctrl_speed_metrics{
  uint8_t curr_speed;
  uint8_t input_speed;
  uint8_t turn; // From turn-regulator
};

/**
 * Input metrics to the TURN-regulator
 */
struct ctrl_turn_metrics{
  uint8_t curr_turn;
  uint8_t input_turn;
  uint8_t dist_left;
  uint8_t dist_right;
};

/**
 * Contains all of the data necessary for the PID-controller
 * to make decisions, e.g telemetrics.
 */
struct ctrl_metrics{

  // *** Speed metrics ***
  bool update_speed_params;
  ctrl_parameters speed_params;
  uint8_t curr_speed;
  uint8_t input_speed;
  
  // *** Turn metrics ***
  bool update_turn_params;
  ctrl_parameters turn_params;
  uint8_t curr_turn;
  uint8_t input_turn;
  uint8_t dist_left;
  uint8_t dist_right;
  
};

// ### Saves the control parameters ###

static ctrl_parameters speed_params_cur =
  {
   .k  = K_SPEED_INITIAL;
   .td = 0; // Not used
  };

static ctrl_parameters turn_params_cur =
  {
   .k  = K_TURN_INITIAL;
   .td = TD_TURN_INITIAL;
  };

// #####################################

/**
 * Main function ran by the pid_controller thread
 * @param metrics Necessary data to perform the regulation
 */
void pid_ctrl_thread_main(ctrl_metrics metrics);

/**
 * Performs the turn regulation
 * @param metrics Necessary data to perform the regulation
 * @return The new turn value
 */
uint8_t regulate_turn(ctrl_turn_metrics metrics);

/**
 * Performs the speed regulation
 * @param metrics Necessary data to perform the regulation
 * @return The new speed value
 */
uint8_t regulate_speed(ctrl_speed_metrics metrics);


#endif
