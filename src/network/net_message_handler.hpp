#ifndef CM_NET_MESSAGE_HANDLER_H
#define CM_NET_MESSAGE_HANDLER_H

#include <vector>

#include "packet.hpp"


/**
 * 
 */
std::vector<packet> handle_packets(const std::vector<packet>& packets);

/**
 * 
 */
packet handle_packet(const packet& p);



#endif