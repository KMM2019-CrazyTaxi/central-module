
#include "update_controller.hpp"

#include <thread>

update_controller::update_controller() : period(DEFAULT_PERIOD) { }

update_controller::update_controller(int64_t _period) : period(_period) {
}

void update_controller::start() {
    start_time = std::chrono::steady_clock::now();
}

void update_controller::wait() {
    
    auto update_period = std::chrono::milliseconds(this->period);

    auto wake_up = start_time + std::chrono::duration_cast<std::chrono::steady_clock::duration>(update_period);

    std::this_thread::sleep_until(wake_up);

}