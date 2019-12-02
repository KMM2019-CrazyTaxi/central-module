#ifndef CM_UPDATE_CONTROLLER_H
#define CM_UPDATE_CONTROLLER_H

#include <cstdint>
#include <chrono>

/**
 * Class that regulates the update frequency of running threads to a given
 * frequency or period
 */
class update_controller {

public:

    // The default period
    const int64_t DEFAULT_PERIOD = 20;

    /**
     * Default constructor
     */
    update_controller();

    /**
     * Constructor that takes a period
     * 
     * @param period The period of the update controller
     */
    update_controller(int64_t period);

    /**
     * Starts the timer of the update controller
     */
    void start();

    /**
     * Blocks the calling thread until period ms has passed since start was called
     */
    void wait();

private:

    // The period of the controller
    int64_t period;

    // The time_point when start() was called
    std::chrono::steady_clock::time_point start_time;

};

#endif