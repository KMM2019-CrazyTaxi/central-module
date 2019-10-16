#include <iostream>
#include <thread>
#include <functional>

#include "io_handler.hpp"

int main () {

    io_data data;
    std::thread io_thread(io_thread_main, std::ref(data));
    
    while (true) {

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    io_thread.join();

    return 0;
}