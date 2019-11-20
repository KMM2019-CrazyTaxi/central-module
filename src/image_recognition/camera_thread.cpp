
#include "camera_thread.hpp"

#include <thread>
#include <chrono>

#ifdef RASPICAM
    #include <raspicam/raspicam.h>
#endif

#include "logging.hpp"

#ifndef RASPICAM
    #include <string>
    #include <fstream>

    #include "image_util.hpp"

    const int N_TEST_IMAGES{8};
    const std::string TEST_IMAGE_PATH = "./src/image_recognition/res/";
    const std::string TEST_IMAGES[N_TEST_IMAGES] = {"1.ppm", "2.ppm", "3.ppm", "4.ppm",
						    "5.ppm", "6.ppm", "7.ppm", "8.ppm"};
#endif

void camera_thread_main(const std::atomic_bool& running, double_buffer& image_buffer) {
    
    #ifdef _RaspiCam_H_
        raspicam::RaspiCam camera;

        camera.setWidth(IMAGE_WIDTH);
        camera.setHeight(IMAGE_HEIGHT);

        image_buffer.resize(camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB));

        if (!camera.open()) queue_message("Failed to open camera.");

        std::this_thread::sleep_for(std::chrono::seconds(3));

    #else

	int n_image{};
	image_buffer.resize(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
	
    #endif

    image_buffer.lock_write_buffer();

    while (running) {

        // Waits until the most recent captured image starts being processed
        // before taking a new one. This function should in theory be enough
        // to handle the lock for the write buffer
        image_buffer.wait_for_image();
        
        // Write image to buffer
        #ifdef _RaspiCam_H_

            camera.grab();
            camera.retrieve(image_buffer.get_write_buffer(), raspicam::RASPICAM_FORMAT_RGB);

        #else

	    // Read sample images if camera isn't available.
	    std::ifstream input{ TEST_IMAGE_PATH + TEST_IMAGES[n_image], std::ios::binary };
	    read_image(image_buffer.get_write_buffer(), input);
	    n_image = (n_image + 1) % N_TEST_IMAGES;
	    input.close();
	    
        #endif
    }

    image_buffer.unlock_write_buffer();

}
