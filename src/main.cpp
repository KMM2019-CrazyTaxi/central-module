#include <iostream>
#include <thread>
#include <string>
#include <atomic>
#include <unordered_map>

#include "io_thread.hpp"
#include "image_recognition.hpp"
#include "logging.hpp"
#include "defs.hpp"
#include "network.hpp"
#include "double_buffer.hpp"
#include "camera_thread.hpp"

int main() {
    
    // Create global running variable
    std::atomic_bool running = true;

    // Create image buffer for image processing
    double_buffer image_buffer(IMAGE_SIZE);

    // Create IO thread for communication with steering and sensor modules
    std::thread io_thread(io_thread_main, std::ref(running));

    // Create network thread for communication with remote module
    std::thread net_thread(network_thread_main, std::ref(running));
    
    // Create camera thread for capturing images
    std::thread cam_thread(camera_thread_main, std::ref(running), std::ref(image_buffer));
    
    // Create image recognition thread for image processing
    std::thread ir_thread(image_recognition_main, std::ref(running), std::ref(image_buffer));

    // Store name for each thread so logging thread knows where messages come from
    std::unordered_map<std::thread::id, std::string> thread_name_map;
    thread_name_map.insert({io_thread.get_id(), IO_THREAD_NAME});
    thread_name_map.insert({ir_thread.get_id(), IR_THREAD_NAME});
    thread_name_map.insert({net_thread.get_id(), NET_THREAD_NAME});
    thread_name_map.insert({cam_thread.get_id(), CAM_THREAD_NAME});
    thread_name_map.insert({std::this_thread::get_id(), MAIN_THREAD_NAME});

    // Create logging thread for printing to stdout and log file
    std::thread logging_thread(logging_thread_main, std::ref(thread_name_map), std::ref(running));

    // running = false;

    io_thread.join();
    ir_thread.join();
    cam_thread.join();
    net_thread.join();
    logging_thread.join();

    return 0;
}