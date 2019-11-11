
#include "io_thread.hpp"
#include "logging.hpp"
#include "data_registry.hpp"
#include "registry_entries.hpp"

#ifdef __WIRING_PI_H__
    #include <wiringPi.h>
    #include <wiringPiSPI.h>
#endif

#include <string.h>

#define SPI_SENSOR_CHANNEL 0
#define SPI_CONTROL_CHANNEL 1

#define SENSOR_MSG_SIZE 13
#define CONTROL_MSG_SIZE 6
#define MSG_BUFFER_SIZE 16

#define SPI_START_BYTE 0xAA

int sensor_failures = 0;
int control_failures = 0;

char control_buffer[MSG_BUFFER_SIZE];
char sensor_buffer[MSG_BUFFER_SIZE];

void acquire_sensor_data() {

    memset(sensor_buffer, 0, MSG_BUFFER_SIZE);
    control_buffer[0] = SPI_START_BYTE;

    #ifdef __WIRING_PI_H__ 
        wiringPiSPIDataRW(SPI_SENSOR_CHANNEL, sensor_buffer, SENSOR_MSG_SIZE);
    #else
        return;
    #endif

    // Check startbyte
    if (sensor_buffer[0] != SPI_START_BYTE) {
        queue_message("Error: Sensor data start byte validation failed. Dropping acquired data.");
        return;
    }

    // Test for checkbyte, if the checkbyte is wrong, drop the data and log an error.
    if (!test_checkbyte(sensor_buffer, SENSOR_MSG_SIZE - 1, sensor_buffer[SENSOR_MSG_SIZE - 1])) {
        queue_message("Error: Sensor data check byte validation failed. Dropping acquired data.");
        return;
    }

    // Otherwise read the data
    short status = concat_bytes(sensor_buffer[1], sensor_buffer[2]);
    short acc_x  = concat_bytes(sensor_buffer[3], sensor_buffer[4]);
    short acc_y  = concat_bytes(sensor_buffer[5], sensor_buffer[6]);
    short acc_z  = concat_bytes(sensor_buffer[7], sensor_buffer[8]);

    char distance = sensor_buffer[9];
    char speed    = sensor_buffer[10];

    return;
}

void send_control_data() {

    memset(control_buffer, 0, MSG_BUFFER_SIZE);
    
    control_change_data data;

    control_change_data* registry_entry = 
        (control_change_data*) data_registry::get_instance().acquire_data(CONTROL_CHANGE_DATA_ID);
    
    data = *registry_entry;
    data_registry::get_instance().release_data(CONTROL_CHANGE_DATA_ID);

    control_buffer[0] = SPI_START_BYTE;
    control_buffer[1] = data.speed_delta;
    control_buffer[2] = data.angle_delta;

    char checkbyte = calc_checkbyte(control_buffer, CONTROL_MSG_SIZE - 1);
    control_buffer[CONTROL_MSG_SIZE - 1] = checkbyte;

    #ifdef __WIRING_PI_H__ 
        wiringPiSPIDataRW(SPI_CONTROL_CHANNEL, control_buffer, SENSOR_MSG_SIZE);
    #else
        return;
    #endif

    if (control_buffer[CONTROL_MSG_SIZE - 1] != checkbyte) {
        queue_message("Error: Control data check byte validation failed.");
    }
}

void io_thread_main(const std::atomic_bool& running) {

    // Set up SPI busses
    #ifdef __WIRING_PI_H__ 
        wiringPiSPISetup(SPI_SENSOR_CHANNEL, SPI_FREQUENCY);
        wiringPiSPISetup(SPI_CONTROL_CHANNEL, SPI_FREQUENCY);
    #endif

    while (running) {

        acquire_sensor_data();
        send_control_data();

        std::this_thread::sleep_for(std::chrono::milliseconds(IO_UPDATE_MS));
    }
}

char calc_checkbyte(char* buffer, int size) {
    
    char acc = buffer[0];
    for (int i = 1; i < size; i++) {
        acc ^= buffer[i];
    }

    return acc;
}

bool test_checkbyte(char* buffer, int size, char checkbyte) {

    char true_checkbyte = calc_checkbyte(buffer, size);
    return true_checkbyte == checkbyte;
}

short concat_bytes(char b1, char b2) {
    return (b1 << 8) + b2;
}