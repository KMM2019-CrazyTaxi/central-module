
#include "image_recognition.hpp"

#include <thread>
#include <chrono>

#include "logging.hpp"

void image_recognition_main(const std::atomic_bool& running) {
    
    // Main loop
    while (running) {

        // Log to stdout and log file
        queue_message("IR Tick");

        // Sleep for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

}