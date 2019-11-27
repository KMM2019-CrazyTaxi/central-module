#ifndef CM_NET_MESSAGE_HANDLER_H
#define CM_NET_MESSAGE_HANDLER_H

#include <vector>

#include "packet.hpp"


/**
 * Handles a vector of packets and returns a vector of packets that contains the answer
 * to the packets.
 * 
 * @param packets A std::vector containing the packets to handle
 * @return A new vector containing the answer packets
 */
std::vector<packet> handle_packets(const std::vector<packet>& packets);

/**
 * Handles a single packet
 * 
 * @param p The packet to handle
 * @return The answer to the packet p
 */
packet handle_packet(const packet& p);

/**
 * 
 */
packet handle_request_sensor_data(const packet& p);

/**
 * 
 */
packet handle_send_max_speed(const packet& p);

/**
 * 
 */
packet handle_request_turn(const packet& p);

/**
 * 
 */
packet handle_request_temperature(const packet& p);

/**
 * 
 */
packet handle_send_current_date_time(const packet& p);

/**
 * 
 */
packet handle_request_control_parameters(const packet& p);

/**
 * 
 */
packet handle_send_control_parameters(const packet& p);

#endif