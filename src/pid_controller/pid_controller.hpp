#ifndef CM_PID_CONTROLLER_H
#define CM_PID_CONTROLLER_H

#include <atomic>
#include "pathfinding.hpp"
#include "graph.hpp"
#include <vector>

/**
 * Main function ran by the pid_controller thread
 * Fetches data for the control system, then sends the
 * task forward to the decision maker.
 */
void pid_ctrl_thread_main(const std::atomic_bool&);

#endif
