#ifndef CM_PID_CONTROLLER_H
#define CM_PID_CONTROLLER_H

#include "defs.hpp"
#include "registry_entries.hpp"
#include <cstdint>
#include <atomic>

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
 * Main function ran by the pid_controller thread
 * @param metrics Necessary data to perform the regulation
 */
void pid_ctrl_thread_main(const std::atomic_bool&);

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
