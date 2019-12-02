
#include "net_message_handler.hpp"
#include "data_registry.hpp"
#include "packet_ids.hpp"
#include "graph.hpp"

#include <stdio.h>
#include <string.h>

packet handle_request_sensor_data(const packet& p);
packet handle_send_max_speed(const packet& p);
packet handle_request_turn(const packet& p);
packet handle_request_temperature(const packet& p);
packet handle_send_current_date_time(const packet& p);
packet handle_request_control_parameters(const packet& p);
packet handle_send_control_parameters(const packet& p);
packet handle_request_control_decision(const packet& p);
packet handle_request_ir_data(const packet& p);
packet handle_send_map(const packet& p);

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
        
        case REQUEST_SENSOR_DATA:
            return handle_request_sensor_data(p);

        case SEND_MAX_SPEED: 
            return handle_send_max_speed(p);
        
        case REQUEST_TURN: 
            return handle_request_turn(p);

        case REQUEST_TEMPERATURE: 
            return handle_request_temperature(p);
        
        case SEND_CURRENT_DATE_TIME:
            return handle_send_current_date_time(p);
        
        case REQUEST_CONTROL_PARAMETERS:
            return handle_request_control_parameters(p);

        case SEND_CONTROL_PARAMETERS: 
            return handle_send_control_parameters(p);

        case REQUEST_CONTROL_DECISION:
            return handle_request_control_decision(p);
        
        case REQUEST_IR_DATA:
            return handle_request_ir_data(p);

        default:
            return packet(p.get_id(), REMOTE_MODULE_COMMUNICATION_ERROR, 0, nullptr);
    }
}

packet handle_request_sensor_data(const packet& p) {
    
    data_registry& registry = data_registry::get_instance();
    sensor_data data = *(sensor_data*) registry.acquire_data(SENSOR_DATA_ID);
    registry.release_data(SENSOR_DATA_ID);

    return packet(p.get_id(), CURRENT_SENSOR_DATA, sizeof(data), (uint8_t*) &data);
}

packet handle_send_max_speed(const packet& p) {

    data_registry& registry = data_registry::get_instance();
    uint8_t speed = p[0];

    control_change_data* registry_entry = 
        (control_change_data*) registry.acquire_data(CONTROL_CHANGE_DATA_ID);

    registry_entry->speed = speed;

    registry.release_data(CONTROL_CHANGE_DATA_ID);

    return packet(p.get_id(), MAX_SPEED_ACKNOWLEDGEMENT, 0, nullptr);
}

packet handle_request_turn(const packet& p) {

    data_registry& registry = data_registry::get_instance();
    uint8_t angle = p[0];

    control_change_data* registry_entry = 
        (control_change_data*) registry.acquire_data(CONTROL_CHANGE_DATA_ID);

    registry_entry->angle = angle;

    registry.release_data(CONTROL_CHANGE_DATA_ID);

    return packet(p.get_id(), TURN_ACKNOWLEDGEMENT, 0, nullptr);
}

packet handle_request_temperature(const packet& p) {

    float systemp, millideg;
    FILE *thermal;
    int n;

    thermal = fopen("/sys/class/thermal/thermal_zone0/temp","r");
    n = fscanf(thermal,"%f",&millideg);
    fclose(thermal);
    systemp = millideg / 1000;

    return packet(p.get_id(), CURRENT_TEMPERATURE, sizeof(float), (uint8_t*) &systemp);
}

packet handle_send_current_date_time(const packet& p) {

    char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "date -s '%s'", (const char*) p.get_data());
    system(buffer);

    return packet(p.get_id(), ACK_CURRENT_DATE_TIME, 0, nullptr);
}

packet handle_request_control_parameters(const packet& p) {

    data_registry& registry = data_registry::get_instance();
    pid_params params;
    uint8_t params_to_get = p.get_data()[0];

    regulator_param_data* reg_params = (regulator_param_data*) registry.acquire_data(REGULATOR_PARAM_DATA_ID);

    switch (params_to_get) {
        case 1:
            params = reg_params->turning;
            break;
        case 2:
            params = reg_params->parking;
            break;
        case 3:
            params = reg_params->stopping;
            break;
        case 4:
            params = reg_params->line_angle;
            break;
        case 5:
            params = reg_params->line_speed;
            break;
    }

    registry.release_data(REGULATOR_PARAM_DATA_ID);
    reg_params = nullptr;
    
    constexpr int PACKET_SIZE = 9 * sizeof(double);

    uint8_t buffer[1 + PACKET_SIZE];

    buffer[0] = params_to_get;
    
    double d_buffer[PACKET_SIZE / sizeof(double)];

    d_buffer[0] = params.kp;
    d_buffer[1] = params.ki;
    d_buffer[2] = params.kd;
    d_buffer[3] = params.alpha;
    d_buffer[4] = params.beta;
    d_buffer[5] = params.angle_threshold;
    d_buffer[6] = params.speed_threshold;
    d_buffer[7] = params.min_value;
    d_buffer[8] = params.slope;

    memcpy(buffer + 1, d_buffer, PACKET_SIZE);

    return packet(p.get_id(), CURRENT_CONTROL_PARAMETERS, PACKET_SIZE, buffer);
}

packet handle_send_control_parameters(const packet& p) {

    data_registry& registry = data_registry::get_instance();
    pid_params params;

    int params_to_get = (int) *p.get_data();

    double buffer[9];
    memcpy(buffer, p.get_data()+sizeof(int), 9 * sizeof(double));

    params.kp               = buffer[0];
    params.ki               = buffer[1];
    params.kd               = buffer[2];
    params.alpha            = buffer[3];
    params.beta             = buffer[4];
    params.angle_threshold  = buffer[5];
    params.speed_threshold  = buffer[6];
    params.min_value        = buffer[7];
    params.slope            = buffer[8];

    regulator_param_data* reg_params = (regulator_param_data*) registry.acquire_data(REGULATOR_PARAM_DATA_ID);

    switch (params_to_get) {
        case 1:
            reg_params->turning = params;
            break;
        case 2:
            reg_params->parking = params;
            break;
        case 3:
            reg_params->stopping = params;
            break;
        case 4:
            reg_params->line_angle = params;
            break;
        case 5:
            reg_params->line_speed = params;
            break;
    }

    registry.release_data(REGULATOR_PARAM_DATA_ID);
    reg_params = nullptr;

    return packet(p.get_id(), PARAMETERS_ACKNOWLEDGEMENT, 0, nullptr);
}

packet handle_request_control_decision(const packet& p) {

    data_registry& registry = data_registry::get_instance();

    regulator_out_data data;
    regulator_out_data* registry_entry = 
        (regulator_out_data*) data_registry::get_instance().acquire_data(REGULATOR_OUT_DATA_ID);
    
    data = *registry_entry;
    data_registry::get_instance().release_data(REGULATOR_OUT_DATA_ID);

    int8_t buffer[2];
    buffer[0] = static_cast<int8_t>(data.speed);
    buffer[1] = static_cast<int8_t>(data.angle);

    return packet(p.get_id(), CURRENT_CONTROL_DECISION, sizeof(buffer), (uint8_t*) buffer);
}

packet handle_request_ir_data(const packet& p) {

    data_registry& registry = data_registry::get_instance();

    telemetrics_data data;
    telemetrics_data* registry_entry = 
        (telemetrics_data*) data_registry::get_instance().acquire_data(REGULATOR_OUT_DATA_ID);
    
    data = *registry_entry;
    data_registry::get_instance().release_data(REGULATOR_OUT_DATA_ID);

    double buffer[3];
    buffer[0] = data.dist_left;
    buffer[1] = data.dist_right;
    buffer[2] = data.dist_stop_line;

    return packet(p.get_id(), CURRENT_IR_DATA, sizeof(buffer), (uint8_t*) buffer);
}

packet handle_send_map(const packet& p) {

    data_registry& registry = data_registry::get_instance();

    graph new_graph = graph(p.get_data());

    mission_data* entry = (mission_data*) registry.acquire_data(MISSION_DATA_ID);

    entry->g = std::move(new_graph);

    registry.release_data(MISSION_DATA_ID);

    return packet(p.get_id(), MAP_ACKNOWLEDGEMENT, 0, nullptr);
}