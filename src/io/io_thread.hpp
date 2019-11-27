#ifndef CM_IO_THREAD_H
#define CM_IO_THREAD_H

#include <atomic>
#include <string>

// The number of milliseconds waited between each io update
#define IO_UPDATE_MS 20

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
bool test_checkbyte(unsigned char* buffer, int size, char checkbyte);

/**
 * Turns a buffer of bytes into a string of hex numbers with spaces between each byte.
 * 
 * @param buffer The buffer to translate
 * @param size The number of bytes to read from the buffer
 * @return A string of the bytes in hexadecimal
 */
std::string print_buffer(uint8_t* buffer, int size);

/**
 * Concatenates two bytes into a short
 */
short concat_bytes(char b1, char b2);

#endif



