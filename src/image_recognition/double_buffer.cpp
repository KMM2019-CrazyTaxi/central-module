
#include "double_buffer.hpp"

#include <algorithm>
#include <mutex>

double_buffer::double_buffer(uint32_t size) /*: buffer_1(size), buffer_2(size) */ {

    #ifdef SHARED_ENABLED
        buffer_1 = SharedArray<uint8_t>(size);
        buffer_2 = SharedArray<uint8_t>(size);
        read_buffer = &buffer_1;
        write_buffer = &buffer_2;
    #else
        read_buffer = new uint8_t[size]();
        write_buffer = new uint8_t[size]();
    #endif

}


double_buffer::~double_buffer() {

    #ifdef SHARED_ENABLED
        read_buffer = nullptr;
        write_buffer = nullptr;
        buffer_1.dealloc();
        buffer_2.dealloc();
    #else
        delete[] read_buffer;
        delete[] write_buffer;
    #endif

}

void double_buffer::resize(uint32_t new_size) {

    write_lock.lock();

    #ifdef SHARED_ENABLED
        read_buffer = nullptr;
        write_buffer = nullptr;
        buffer_1.dealloc();
        buffer_2.dealloc();
    #else
        delete[] read_buffer;
        delete[] write_buffer;
    #endif

    #ifdef SHARED_ENABLED
        buffer_1 = SharedArray<uint8_t>(new_size);
        buffer_2 = SharedArray<uint8_t>(new_size);
        read_buffer = &buffer_1;
        write_buffer = &buffer_2;
    #else
        read_buffer = new uint8_t[new_size]();
        write_buffer = new uint8_t[new_size]();
    #endif

    write_lock.unlock();
}

uint8_t double_buffer::read(uint32_t index) const {

    #ifdef SHARED_ENABLED
        return (*read_buffer)[index];
    #else
        return read_buffer[index];
    #endif

}

void double_buffer::write(uint32_t index, uint8_t value) {

    #ifdef SHARED_ENABLED
        (*write_buffer)[index] = value;
    #else
        write_buffer[index] = value;
    #endif

}

const uint8_t* double_buffer::get_read_buffer() const {

    #ifdef SHARED_ENABLED
        return read_buffer->getPointer();
    #else
        return read_buffer;
    #endif
}

uint8_t* double_buffer::get_write_buffer() {

    #ifdef SHARED_ENABLED
        return write_buffer->getPointer();
    #else
        return write_buffer;
    #endif
}

void double_buffer::lock_write_buffer() {
    write_lock.lock();
}

void double_buffer::unlock_write_buffer() {
    write_lock.unlock();
}

void double_buffer::swap_buffers() {

    write_lock.lock();

    std::swap(read_buffer, write_buffer);
    image_processed = true;

    write_lock.unlock();
    new_image_condition.notify_one();

}

void double_buffer::wait_for_image() {

    // Create unique lock from writer lock without locking it, since it is already locked by this thread
    std::unique_lock lock(write_lock, std::adopt_lock);
    new_image_condition.wait(lock, [this]{ return this->image_processed.load(); });
    
    image_processed = false;

    // Release association with the writer_lock, note that it is still locked after this
    lock.release();
}