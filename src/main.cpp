#include <iostream>
#include <thread>

#include "io_thread.hpp"
#include "image_recognition.hpp"
#include "logging.hpp"

int main() {

    std::thread io_thread(io_thread_main);
    std::thread logging_thread(logging_thread_main);
    std::thread ir_thread(image_recognition_main);

    return 0;
}