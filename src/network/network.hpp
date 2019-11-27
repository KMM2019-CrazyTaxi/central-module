#ifndef CM_NETWORK_H
#define CM_NETWORK_H

#include <atomic>
#include <vector>

#include "packet.hpp"

// The size of the network buffer
#define NETWORK_BUFFER_SIZE 4096

// The port used for communication
#define PORT 20001

// The number of maximum connections for the server
#define MAX_CONNECTIONS 1

/**
 * Main function ran by the network thread
 */
void network_thread_main(const std::atomic_bool& running);

/**
 * Tries creating a socket file descriptor
 * 
 * @return the socket file descriptor
 * @param tries if socket creation fails, how many times should it try
 */
int create_socket(int tries);

/**
 * Parses a number of packets from the buffer
 * 
 * @param buffer The buffer containing the message received from the remote module
 * @param buffer_size The size of the buffer
 * @return A std::vector with all the parsed packets 
 */
std::vector<packet> parse_packets(uint8_t* buffer, uint32_t buffer_size);

/**
 * Parses a single packet from the buffer
 * 
 * @param buffer The buffer containing the message received from the remote module
 * @param buffer_size The size of the buffer
 * @return The parsed packet
 */
packet parse_packet(uint8_t* buffer, uint32_t buffer_size);

#endif
