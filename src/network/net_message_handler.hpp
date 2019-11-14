#ifndef CM_NET_MESSAGE_HANDLER_H
#define CM_NET_MESSAGE_HANDLER_H

#include <vector>

#include "packet.hpp"


void handle_packets(const std::vector<packet>& packets);

void handle_packet(const packet& p);

#endif