
#include "io_thread.hpp"
#include "logging.hpp"
#include "data_registry.hpp"
#include "registry_entries.hpp"

#ifdef WIRING_PI
    #include <wiringPi.h>
    #include <wiringPiSPI.h>
#endif

#include <string.h>
#include <sstream>

#include "spi.hpp"

#define SENSOR_MSG_SIZE 13
#define CONTROL_MSG_SIZE 4
#define MSG_BUFFER_SIZE 16

#define CONTROL_MSG_CHECKBYTE 3

int sensor_failures = 0;
int control_failures = 0;

char sensor_buffer[MSG_BUFFER_SIZE];

void activate_slave(int slave);
void deactivate_slave(int slave);
void set_slave(int slave, int val);

void acquire_sensor_data() {

    memset(sensor_buffer, 0, MSG_BUFFER_SIZE);

    #ifdef __WIRING_PI_H__
        activate_slave(SPI_SENSOR);
        wiringPiSPIDataRW(SPI_CHANNEL, (unsigned char*) sensor_buffer, SENSOR_MSG_SIZE);
        deactivate_slave(SPI_SENSOR);
    #else
        return;
    #endif

    // Check startbyte
    if (sensor_buffer[0] != SPI_START) {
        #ifdef __WIRING_PI_H__
            queue_message("Error: Sensor data start byte validation failed. Dropping acquired data.");
        #endif
        return;
    }

    // Test for checkbyte, if the checkbyte is wrong, drop the data and log an error.
    if (!test_checkbyte(sensor_buffer, SENSOR_MSG_SIZE - 1, sensor_buffer[SENSOR_MSG_SIZE - 1])) {
        #ifdef __WIRING_PI_H__
            queue_message("Error: Sensor data check byte validation failed. Dropping acquired data.");
        #endif
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

std::string print_buffer(uint8_t* buffer, int size) {
    std::stringstream s;
    for (int i = 0; i < size; i++) {
        s << std::hex << (int) buffer[i] << " ";
    }
    s << std::endl;
    return s.str();
}

void send_control_data() {

    char msg_buffer[MSG_BUFFER_SIZE];
    char ans_buffer[1];

    int fails = 0;
    while (fails < SPI_FAIL_COUNT) {
        memset(msg_buffer, 0, MSG_BUFFER_SIZE);
        
        control_change_data data;

        control_change_data* registry_entry = 
            (control_change_data*) data_registry::get_instance().acquire_data(CONTROL_CHANGE_DATA_ID);
        
        data = *registry_entry;
        data_registry::get_instance().release_data(CONTROL_CHANGE_DATA_ID);

        msg_buffer[0] = SPI_START;
        msg_buffer[1] = 1; // data.speed_delta;
        msg_buffer[2] = 2; // data.angle_delta;
        msg_buffer[3] = SPI_NAN; // NaN

        char checkbyte = calc_checkbyte(msg_buffer, CONTROL_MSG_SIZE - 1);

        queue_message("Transmitted " + print_buffer((uint8_t*) msg_buffer, 4));


        #ifdef __WIRING_PI_H__ 
            activate_slave(SPI_CONTROL);
            wiringPiSPIDataRW(SPI_CHANNEL, (unsigned char*) msg_buffer, CONTROL_MSG_SIZE);
            deactivate_slave(SPI_CONTROL);
        #endif

        queue_message("Received " + print_buffer((uint8_t*) msg_buffer, 4));

        unsigned char answer = SPI_FINISHED;

        if (msg_buffer[CONTROL_MSG_CHECKBYTE] != checkbyte) {
            #ifdef __WIRING_PI_H__
                queue_message("Error: Control data check byte validation failed.");
                answer = SPI_RESTART;
            #endif
        }

        queue_message("Transmitted " + print_buffer((uint8_t*) &answer, 1));
        
        // Write answer
        #ifdef __WIRING_PI_H__ 
            activate_slave(SPI_CONTROL);
            wiringPiSPIDataRW(SPI_CHANNEL, &answer, 1);
            deactivate_slave(SPI_CONTROL);
        #endif

        if (answer == SPI_FINISHED) break;

    }
}

void io_thread_main(const std::atomic_bool& running) {

    // Set up SPI busses
    #ifdef __WIRING_PI_H__ 
        queue_message("Setting up SPI channels.");
        wiringPiSetup();
        wiringPiSPISetup(0, SPI_FREQ);
        pinMode(SPI_CONTROL_SS_PIN, OUTPUT);
        pinMode(SPI_SENSOR_SS_PIN, OUTPUT);
    #endif

    while (running) {

        // acquire_sensor_data();
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

void set_slave(int slave, int val);

void activate_slave(int slave) {
    set_slave(slave, 0);
}

void deactivate_slave(int slave) {
    set_slave(slave, 1);
}

void set_slave(int slave, int val) {
    int pin;

    switch (slave) {
        case SPI_CONTROL:
            pin = SPI_CONTROL_SS_PIN;
            break;
        case SPI_SENSOR:
            pin = SPI_SENSOR_SS_PIN;
            break;
    }

    #ifdef __WIRING_PI_H__ 
        digitalWrite(pin, val);
    #endif
}

bool test_checkbyte(char* buffer, int size, char checkbyte) {

    char true_checkbyte = calc_checkbyte(buffer, size);
    return true_checkbyte == checkbyte;
}

short concat_bytes(char b1, char b2) {
    return (b1 << 8) + b2;
}