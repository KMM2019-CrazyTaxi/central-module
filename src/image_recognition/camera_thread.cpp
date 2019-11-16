
#include "camera_thread.hpp"


void camera_thread_main(const std::atomic_bool& running, double_buffer& image_buffer) {

    while (running) {

        // Waits until the most recent captured image starts being processed
        // before taking a new one. This function should in theory be enough
        // to handle the lock for the write buffer
        image_buffer.wait_for_image();
        
        // Write image to buffer
    }

}