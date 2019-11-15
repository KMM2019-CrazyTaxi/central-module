
#include "net_message_handler.hpp"
#include "data_registry.hpp"
#include "packet_ids.hpp"

#include <stdio.h>

std::vector<packet> handle_packets(const std::vector<packet>& packets) {

    std::vector<packet> result;

    for (const packet& p : packets) {
        result.push_back(handle_packet(p));
    }

    return result;
}

packet handle_packet(const packet& p) {

    data_registry& registry = data_registry::get_instance();

    
    switch(p.get_type()) {
        case SEND_MAX_SPEED: {

            uint8_t speed = p[0];

            control_change_data* registry_entry = 
                (control_change_data*) registry.acquire_data(CONTROL_CHANGE_DATA_ID);

            registry_entry->speed = speed;

            registry.release_data(CONTROL_CHANGE_DATA_ID);

            return packet(p.get_id(), MAX_SPEED_ACKNOWLEDGEMENT, 0, nullptr);
        }
        
        case REQUEST_TURN: {
            uint8_t angle = p[0];

            control_change_data* registry_entry = 
                (control_change_data*) registry.acquire_data(CONTROL_CHANGE_DATA_ID);

            registry_entry->angle = angle;

            registry.release_data(CONTROL_CHANGE_DATA_ID);

            return packet(p.get_id(), TURN_ACKNOWLEDGEMENT, 0, nullptr);
        }

        case REQUEST_TEMPERATURE: {
            float systemp, millideg;
            FILE *thermal;
            int n;

            thermal = fopen("/sys/class/thermal/thermal_zone0/temp","r");
            n = fscanf(thermal,"%f",&millideg);
            fclose(thermal);
            systemp = millideg / 1000;

            return packet(p.get_id(), CURRENT_TEMPERATURE, sizeof(float), (uint8_t*) &systemp);
        }
        default:
            return packet(p.get_id(), REMOTE_MODULE_COMMUNICATION_ERROR, 0, nullptr);
    }
}