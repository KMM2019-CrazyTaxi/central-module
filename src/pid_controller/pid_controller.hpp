#ifndef CM_PID_CONTROLLER_H
#define CM_PID_CONTROLLER_H

#include <atomic>

/**
 * Main function ran by the pid_controller thread
 */
void pid_ctrl_thread_main(const std::atomic_bool&);

#endif
