
#include "packet.hpp"

#include <algorithm>
#include <stdint.h>

packet::packet(uint32_t _id, uint32_t _type, uint32_t _size, uint8_t* buffer) {

    id   = _id;
    type = _type;
    size = _size;
    data = new uint8_t[_size]();

    std::copy(buffer, buffer+size, data);
}

packet::packet(const packet& other) {

    id   = other.id;
    type = other.type;
    size = other.size;

    std::copy(other.data, other.data + other.size, data);
}

packet& packet::operator=(const packet& other) {
    packet temp(other);
    swap(*this, temp);
    return *this;
}

packet::packet(packet&& other) : packet() {
    swap(*this, other);
}

packet& packet::operator=(packet&& other) {
    swap(*this, other);
    return *this;
}

packet::~packet() {
    delete[] data;
}

uint32_t packet::get_id() const {
    return id;
}

uint32_t packet::get_size() const {
    return size;
}

uint32_t packet::get_type() const {
    return type;
}

uint8_t packet::operator[](uint32_t index) const {
    return data[index];
}

void packet::write(const packet& p, uint8_t* buffer) {

    buffer[0] = (uint8_t) p.type;
    buffer[1] = ((uint8_t*) &p.id)[0]; 
    buffer[2] = ((uint8_t*) &p.id)[1]; 
    buffer[3] = ((uint8_t*) &p.size)[0]; 
    buffer[4] = ((uint8_t*) &p.size)[1]; 
    
    std::copy(p.data, p.data + p.size, buffer+5);
  
}

void packet::swap(packet& lhs, packet& rhs) {

    std::swap(lhs.id,   rhs.id);
    std::swap(lhs.size, rhs.size);
    std::swap(lhs.type, rhs.type);
    std::swap(lhs.data, rhs.data);
}