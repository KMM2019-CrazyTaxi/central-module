#ifndef CM_IO_THREAD_H
#define CM_IO_THREAD_H

#include <atomic>

// The number of milliseconds waited between each io update
#define IO_UPDATE_MS 1000

/**
 * Main function ran by the io thread
 */
void io_thread_main(const std::atomic_bool& running);

/**
 * Acquires sensor data from the sensor module and updates the registry
 */
void acquire_sensor_data();

/**
 * Sends new control directives to the control module
 */
void send_control_data();

/**
 * Calculates the checkbyte for the given data using xor
 * 
 * @param buffer the buffer to hash
 * @param size the number of bytes from buffer to hash
 * @return the checkbyte of the data
 */
char calc_checkbyte(unsigned char* buffer, int size);

/**
 * Checks whether the given checkbyte matches the buffer
 * 
 * @param buffer the buffer to hash
 * @param size the number of bytes from buffer to hash
 * @param checkbyte the expected hash of the data
 * @return whether the given checkbyte matches the hash
 */
bool test_checkbyte(char* buffer, int size, char checkbyte);

/**
 * Concatenates two bytes into a short
 */
short concat_bytes(char b1, char b2);

#endif