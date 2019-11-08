#ifndef CM_NETWORK_H
#define CM_NETWORK_H

#include <atomic>

// The size of the network buffer
#define NETWORK_BUFFER_SIZE 1024

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

#endif
