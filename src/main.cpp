#include <iostream>
#include <thread>
#include <string>
#include <atomic>
#include <unordered_map>

#include "io_thread.hpp"
#include "image_recognition.hpp"
#include "logging.hpp"
#include "defs.hpp"

int main() {

    std::atomic_bool running = true;

    std::thread io_thread(io_thread_main, std::ref(running));
    std::thread ir_thread(image_recognition_main);

    std::unordered_map<std::thread::id, std::string> thread_name_map;
    thread_name_map.insert({io_thread.get_id(), IO_THREAD_NAME});
    thread_name_map.insert({ir_thread.get_id(), IR_THREAD_NAME});
    thread_name_map.insert({std::this_thread::get_id(), MAIN_THREAD_NAME});

    std::thread logging_thread(logging_thread_main, std::ref(thread_name_map), std::ref(running));

    queue_message("Message 1");
    queue_message("Message 2");
    queue_message("Message 3");
    queue_message("Message 4");

    // running = false;

    io_thread.join();
    ir_thread.join();
    logging_thread.join();

    return 0;
}