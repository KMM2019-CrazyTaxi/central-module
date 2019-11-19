
#include "data_registry.hpp"
#include "registry_entries.hpp"
#include "defs.hpp"

data_registry::data_registry() {

    // Initialise sensor data entry
    sensor_data* sd = new sensor_data();
    registry[SENSOR_DATA_ID].id = SENSOR_DATA_ID;
    registry[SENSOR_DATA_ID].data = (void*) sd;

    control_change_data* ccd = new control_change_data();
    registry[CONTROL_CHANGE_DATA_ID].id = CONTROL_CHANGE_DATA_ID;
    registry[CONTROL_CHANGE_DATA_ID].data = (void*) ccd;

    regulator_data* rd = new regulator_data();
    // Set initial values
    rd->speed_params =
      {
       .k = PID_K_SPEED_INITIAL,
       .td = 0 // Unused
      };
    rd->turn_params =
      {
       .k = PID_K_TURN_INITIAL,
       .td = PID_TD_TURN_INITIAL
      };
    registry[REGULATOR_DATA_ID].id = REGULATOR_DATA_ID;
    registry[REGULATOR_DATA_ID].data = (void*) rd;

    regulator_output* ro = new regulator_output();
    registry[REGULATOR_OUTPUT_ID].id = REGULATOR_OUTPUT_ID;
    registry[REGULATOR_OUTPUT_ID].data = (void*) ro;

}

data_registry::~data_registry() {

    for (auto& pair : registry) {
        pair.second.lock.lock();
        delete pair.second.data;
        pair.second.lock.unlock();
    }
}

data_registry& data_registry::get_instance() {
    static data_registry instance;
    return instance;
}

void* data_registry::acquire_data(const std::string& id) {

    void* data;

    // Check if entry exists
    if (!registry.count(id)) {
        // TODO: Throw exception or print to IO-thread?
        return nullptr;
    }

    registry_entry& entry = registry[id];
    entry.lock.lock();
    data = entry.data;

    return data;
}

void data_registry::release_data(const std::string& id) {

    // Check if entry exists
    if (!registry.count(id)) {
        return;
    }

    registry_entry& entry = registry[id];
    entry.lock.unlock();
    
}
