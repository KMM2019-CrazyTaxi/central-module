
#include "data_registry.hpp"
#include "registry_entries.hpp"

data_registry::data_registry() {

    // Initialise sensor data entry
    sensor_data* sd = new sensor_data();
    registry[SENSOR_DATA_ID].id = SENSOR_DATA_ID;
    registry[SENSOR_DATA_ID].data = (void*) sd;

}

data_registry::~data_registry() {

    global_lock.lock();
    
    for (auto& pair : registry) {
        pair.second.lock.lock();
        delete pair.second.data;
        pair.second.lock.unlock();
    }

    global_lock.unlock();
}

data_registry& data_registry::get_instance() {
    static data_registry instance;
    return instance;
}

void* data_registry::acquire_data(const std::string& id) {

    void* data;
    global_lock.lock();

    // Check if entry exists
    if (!registry.count(id)) {
        // TODO: Throw exception or print to IO-thread?
        global_lock.unlock();
        return nullptr;
    }

    registry_entry& entry = registry[id];
    entry.lock.lock();
    data = entry.data;

    entry.lock.unlock();
    global_lock.unlock();

    return data;
}

void data_registry::release_data(const std::string& id) {

    global_lock.lock();

    // Check if entry exists
    if (!registry.count(id)) {
        global_lock.unlock();
        return;
    }

    registry_entry& entry = registry[id];
    entry.lock.unlock();

    global_lock.unlock();
}