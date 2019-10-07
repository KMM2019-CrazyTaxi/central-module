#ifndef CM_IO_HANDLER_H
#define CM_IO_HANDLER_H

#include <atomic>

// Structure that contains all gpio data wrapped in atomic structs 
struct io_data {

    std::atomic<float> distance;
    std::atomic<float> acceleration;

};

// Main function of the io thread
int io_thread_main(io_data& data);

// Get distance from laser sensor
static float update_distance();

// Get acceleration from accelerometer
static float update_acceleration();


#endif