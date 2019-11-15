#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <string>
#include <sstream>

#include "net_message_handler.hpp"
#include "network.hpp"
#include "logging.hpp"
#include "defs.hpp"
#include "io_thread.hpp"

uint16_t concat_bytes(uint8_t hi, uint8_t lo);

int create_socket(int max_tries) {

    queue_message("Creating socket file descriptor...");
    int server_fd = 0;

    int tries = 0;
    while ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        
        queue_message("Failed to create socket. Try #" + std::to_string(tries));
        
        if (tries == max_tries - 1) break;
        tries++;

    }

    if (server_fd == 0) {
        queue_message("Failed socket creation after " + std::to_string(tries + 1) + " tries");
        return -1;
    }

    queue_message("Socket created.");
    return server_fd;
}

void network_thread_main(const std::atomic_bool& running) {

    int server_fd = 0;
    int valread;

    sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(sockaddr_in);
    char buffer[NETWORK_BUFFER_SIZE] = {0};

    #define SOCKET_TRIES 3
    server_fd = create_socket(SOCKET_TRIES);

    if (server_fd == -1) return;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        queue_message("Failed setting options for socket.");
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Attach the socket to port PORT
    if (bind(server_fd, (sockaddr*) &address, sizeof(address)) < 0) {
        queue_message("Failed binding socket to port " + std::to_string(PORT));
        return;
    }

    // Listen for connections on the socket
    int listen_error_code;
    if ((listen_error_code = listen(server_fd, MAX_CONNECTIONS)) < 0) {
        queue_message("Failed listening for connections on socket. Error code " + std::to_string(listen_error_code));
        return;
    }
    
    queue_message("Server set up. Waiting for connection...");

    int new_socket;
    bool connection = false;

    while (running) {

        // Try accepting a connection
        if ((new_socket = accept(server_fd, (sockaddr*) &address, (socklen_t*) &addrlen)) < 0) {
            queue_message("Failed accepting connections. Error code " + std::to_string(new_socket));
            continue;
        } else {
            connection = true;
        }

        std::stringstream msg;

        msg << "Connection from " 
            << std::string(inet_ntoa(address.sin_addr)) 
            << " port " 
            << std::to_string(ntohs(address.sin_port));

        queue_message(msg.str());
        msg.str("");

        while (connection) {
            
            valread = read(new_socket, buffer, NETWORK_BUFFER_SIZE);
            
            std::vector<packet> read_packets;
            std::vector<packet> answer_packets;

            if (valread > 0) {

                msg << "Read " << valread << " bytes from " << std::string(inet_ntoa(address.sin_addr));
                queue_message(msg.str());
                msg.str("");

                queue_message(print_buffer((uint8_t*) buffer, valread));

                read_packets = parse_packets((uint8_t*) buffer, NETWORK_BUFFER_SIZE);
                msg << "Parsed " << read_packets.size() << " packets";
                queue_message(msg.str());
                msg.str("");

                answer_packets = handle_packets(read_packets);
            }

            const char* hello = "Hello from server";

            memset(buffer, 0, NETWORK_BUFFER_SIZE);
            buffer[0] = (uint8_t) answer_packets.size();

            uint8_t* local_buffer = (uint8_t*) buffer + 1;

            for (const auto& packet : answer_packets) {
                packet::write(packet, local_buffer);
                local_buffer += PACKET_HEADER_SIZE + packet.get_size();
            }

            uint32_t answer_size = (uint32_t) (local_buffer - (uint8_t*) buffer);

            queue_message(print_buffer((uint8_t*) buffer, answer_size));

            int send_error = send(new_socket, buffer, answer_size, MSG_NOSIGNAL);

            if (send_error == -1) {
                connection = false;
                queue_message("Connection broken.");
            }

            if (!running) {
                if (connection) close(new_socket);
                close(server_fd);
            }
        }
    }
}

std::vector<packet> parse_packets(uint8_t* buffer, uint32_t buffer_size) {

    uint32_t buffer_index = 0;

    int packet_count = (int) buffer[buffer_index++];
    std::vector<packet> packets;

    for (int i = 0; i < packet_count; i++) {
        
        packet p = parse_packet(buffer + buffer_index, buffer_size);
        // Increase the buffer index with 5 (packet header) and packet size
        buffer_index += PACKET_HEADER_SIZE + p.get_size();
        
        packets.push_back(p);
    }

    return packets;
}

packet parse_packet(uint8_t* buffer, uint32_t buffer_size) {

    uint32_t type = (uint32_t) buffer[0];
    uint32_t id   = (uint32_t) concat_bytes(buffer[2], buffer[1]);
    uint32_t size = (uint32_t) concat_bytes(buffer[4], buffer[3]);

    packet p = packet(id, type, size, buffer + PACKET_HEADER_SIZE);

    return p;
}

uint16_t concat_bytes(uint8_t hi, uint8_t lo) {
    return (hi << 8) + lo;
}