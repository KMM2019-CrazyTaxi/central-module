#ifndef CM_DOUBLE_BUFFER_H
#define CM_DOUBLE_BUFFER_H

#include <cstdint>
#include <atomic>
#include <mutex>
#include <condition_variable>

#ifdef QPULIB
    #include "QPULib.h"
#endif

#ifdef _QPULIB_H_
    #define SHARED_ENABLED
#endif

/**
 * A double buffer class used to allow synchronous image reading and writing
 */
class double_buffer {

public:

    /**
     * Constructor that creates a double_buffer with a given size
     * 
     * @param size The size of the buffer in bytes
     */
    double_buffer(uint32_t size);

    /**
     * Destructor
     */
    ~double_buffer();

    /**
     * Resizes the double buffer to the new size
     * 
     * @param new_size The new size of the double buffer
     */
    void resize(uint32_t new_size);

    /**
     * Reads a byte from the read-only buffer
     * 
     * @param index The index to read from
     */
    uint8_t read(uint32_t index) const;

    /**
     * Writes a byte to the write-only buffer
     * 
     * @param index The index to write to
     * @param value The value to write
     */
    void write(uint32_t index, uint8_t value);

    /**
     * @return Constant pointer to the read-only buffer
     */
    const uint8_t* get_read_buffer() const;

    /**
     * @return Pointer to the write-only buffer
     */
    uint8_t* get_write_buffer();

    /**
     * Swaps the read and write buffers
     */
    void swap_buffers();

    /**
     * Calling this function blocks the calling thread until swap_buffers is called.
     * NOTE: Keeps the write buffer locked, make sure to unlock it when you are done
     */
    void wait_for_image();


private:

    /**
     * Explicitly deleted default constructor and copy and move semantics
     */
    double_buffer() = delete;
    double_buffer(const double_buffer& other) = delete;
    double_buffer(double_buffer&& other) = delete;
    double_buffer& operator=(const double_buffer& other) = delete;
    double_buffer& operator=(double_buffer&& other) = delete;

    // Locks the write buffer 
    std::mutex write_lock;

    // Condition that wakes up camera thread when the image has been processed
    std::condition_variable new_image_condition;

    // Flag for new image
    std::atomic_bool image_processed;

    #ifdef SHARED_ENABLED

        // Buffers are shared with the GPU
        SharedArray<uint8_t> buffer_1;
        SharedArray<uint8_t> buffer_2;
        
        // Pointers to the shared buffers to allow swapping
        SharedArray<uint8_t>* read_buffer;
        SharedArray<uint8_t>* write_buffer;

    #else
        
        // CPU Only buffers
        uint8_t* read_buffer;
        uint8_t* write_buffer;

    #endif

};

#endif