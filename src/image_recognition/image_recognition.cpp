
#include "image_recognition.hpp"

#include <thread>
#include <chrono>

#include "logging.hpp"
#include "double_buffer.hpp"

void image_recognition_main(const std::atomic_bool& running, double_buffer& image_buffer) {

    // Swap the buffers to notify camera thread to grab image
    image_buffer.swap_buffers();

    // Main loop
    while (running) {

        // Log to stdout and log file
        queue_message("IR Tick");

        // Sleep for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

}