#ifndef CM_NET_PACKET_H
#define CM_NET_PACKET_H

#include <stdint.h>

#define PACKET_HEADER_SIZE 5

class packet {
public:

    /**
     * Constructs a packet
     * 
     * @param id The unique identifier of the packet
     * @param type The type of the packet
     * @param size The size of the packet in bytes
     * @param buffer The buffer to copy the data from  
     */
    packet(uint32_t id, uint32_t type, uint32_t size, uint8_t* buffer);

    /**
     * Copy constructor
     */
    packet(const packet& other);

    /**
     * Copy assignment operator
     */
    packet& operator=(const packet& other);

    /**
     * Move constructor
     */
    packet(packet&& other);

    /**
     * Move assignment operator
     */
    packet& operator=(packet&& other);

    /**
     * Destructor, frees the buffer
     */
    ~packet();

    /**
     * @return The unique identifier of the packet
     */
    uint32_t id() const;

    /**
     * @return The size of the data contained in the packet
     */
    uint32_t size() const;

    /**
     * @return The type of the packet
     */
    uint32_t type() const;

    /**
     * Indexes over the data
     */
    // uint8_t operator[](uint32_t index) const;

    /**
     * Writes the given packet to the given buffer
     * 
     * @param p The packet to write
     * @param buffer The buffer to write to
     */
    static void write(const packet& p, uint8_t* buffer);

private:

    // The unique identifier of the packet
    uint32_t id;

    // The size of the data contained in the packet
    uint32_t size;

    // The type of the packet
    uint32_t type;

    // Pointer to the data of the packet
    uint8_t* data;

    /**
     *  Private default constructor
     */
    packet() = default;

    /**
     * Swaps two packet objects, used for the rule of four (and a half)
     */
    static void swap(packet& lhs, packet& rhs);
};

#endif