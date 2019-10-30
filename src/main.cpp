#include <iostream>
#include <thread>
#include <functional>

#include "data_registry.hpp"
#include "registry_entries.hpp"

void writer_main() {

    data_registry& registry = data_registry::get_instance();

    while (true) {

        sensor_data* sd = (sensor_data*) registry.acquire_data(SENSOR_DATA_ID);
        sd->acceleration++;
        sd->distance++;
    
        registry.release_data(SENSOR_DATA_ID);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main () {

    std::thread writer_thread(writer_main);
    
    data_registry& registry = data_registry::get_instance();
    while (true) {

        sensor_data* sd = (sensor_data*) registry.acquire_data(SENSOR_DATA_ID);
        std::cout << "Acceleration: " << sd->acceleration << " Distance: " << sd->distance << std::endl;
    
        registry.release_data(SENSOR_DATA_ID);
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    writer_thread.join();

    return 0;
}