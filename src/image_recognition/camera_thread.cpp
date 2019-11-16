
#include "camera_thread.hpp"

#include <thread>
#include <chrono>

#ifdef RASPICAM
    #include <raspicam/raspicam.h>
#endif

#include "logging.hpp"

void camera_thread_main(const std::atomic_bool& running, double_buffer& image_buffer) {
    
    #ifdef _RaspiCam_H_
        raspicam::RaspiCam camera;

        image_buffer = double_buffer(camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB));

        if (!camera.open()) queue_message("Failed to open camera.");

        std::this_thread::sleep_for(std::chrono::seconds(3));
    #endif

    while (running) {

        // Waits until the most recent captured image starts being processed
        // before taking a new one. This function should in theory be enough
        // to handle the lock for the write buffer
        image_buffer.wait_for_image();
        
        // Write image to buffer
        #ifdef _RaspiCam_H_

            camera.grab();
            camera.retrieve(image_buffer.get_write_buffer(), raspicam::RASPICAM_FORMAT_RGB);

        #endif
    }

}