
#include "net_message_handler.hpp"
#include "data_registry.hpp"
#include "packet_ids.hpp"

void handle_packets(const std::vector<packet>& packets) {

    for (const packet& p : packets) {
        handle_packet(p);
    }

}

void handle_packet(const packet& p) {

    data_registry& registry = data_registry::get_instance();

    switch(p.get_type()) {
        case SEND_MAX_SPEED: {

            uint8_t speed = p[0];

            control_change_data* registry_entry = 
                (control_change_data*) registry.acquire_data(CONTROL_CHANGE_DATA_ID);

            registry_entry->speed = speed;

            registry.release_data(CONTROL_CHANGE_DATA_ID);
            break;
        }
        
        case REQUEST_TURN: {
            uint8_t angle = p[0];

            control_change_data* registry_entry = 
                (control_change_data*) registry.acquire_data(CONTROL_CHANGE_DATA_ID);

            registry_entry->angle = angle;

            registry.release_data(CONTROL_CHANGE_DATA_ID);
            break;
        }
        default:
            break;
    }
}