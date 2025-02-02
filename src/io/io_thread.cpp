
#include "io_thread.hpp"
#include "logging.hpp"
#include "data_registry.hpp"
#include "registry_entries.hpp"
#include "update_controller.hpp"

#ifdef WIRING_PI
    #include <wiringPi.h>
    #include <wiringPiSPI.h>
#endif

#include <string.h>
#include <sstream>

#include "spi.hpp"

#define MSG_BUFFER_SIZE 16

int sensor_failures = 0;
int control_failures = 0;

void activate_slave(int slave);
void deactivate_slave(int slave);
void set_slave(int slave, int val);

void spi_write(int slave, unsigned char* buffer, int size);

void acquire_sensor_data() {
    
    unsigned char start_buffer[SPI_SENSOR_INIT_MSG_SIZE];
    unsigned char confirm_buffer[SPI_SENSOR_CONFIRM_MSG_SIZE];
    unsigned char msg_buffer[SPI_SENSOR_DATA_MSG_SIZE];
    unsigned char ans_buffer[SPI_SENSOR_FINISH_MSG_SIZE];

    int fails = 0;
    while (fails < SPI_FAIL_COUNT) {

        start_buffer[0] = SPI_START_SENSOR;
        start_buffer[1] = 0x00;

        // If wiring pi library is not present, return
        #ifndef __WIRING_PI_H__
            return;    
        #endif

        spi_write(SPI_SENSOR, start_buffer, SPI_SENSOR_INIT_MSG_SIZE);

        // Check answer byte
        if (start_buffer[1] != SPI_ACK) {
                queue_message("Failed initialising communication with sensor module. Retrying in 1 ms");

                std::this_thread::sleep_for(std::chrono::milliseconds(SPI_FAILED_WAIT_MS));
                fails++;
                continue;
        }

        // Send success confirm byte
        confirm_buffer[0] = SPI_CONFIRM;
        spi_write(SPI_SENSOR, confirm_buffer, SPI_SENSOR_CONFIRM_MSG_SIZE);

        // Read the sensor data
        spi_write(SPI_SENSOR, msg_buffer, SPI_SENSOR_DATA_MSG_SIZE);

        unsigned char answer = SPI_FINISHED;

        unsigned char checkbyte = msg_buffer[SPI_SENSOR_DATA_MSG_SIZE - 1];
        unsigned char expected_checkbyte = calc_checkbyte(msg_buffer, SPI_SENSOR_DATA_MSG_SIZE - 1);

        // Test for checkbyte, if the checkbyte is wrong, drop the data and log an error.
        if (checkbyte != expected_checkbyte) {
            queue_message(
                "Error: Sensor data check byte validation failed. Got " + 
                std::to_string(checkbyte) + 
                " expected " +
                std::to_string(expected_checkbyte) +
                ". Retrying communication.");
            answer = SPI_RESTART;
            fails++;
        }

        // Write answer
        ans_buffer[0] = answer;
        spi_write(SPI_SENSOR, ans_buffer, SPI_SENSOR_FINISH_MSG_SIZE);

        if (answer == SPI_RESTART) continue;

        sensor_data result;

        // Otherwise read the data
        short status = concat_bytes(msg_buffer[1], msg_buffer[0]);
        short acc_x  = concat_bytes(msg_buffer[3], msg_buffer[2]);
        short acc_y  = concat_bytes(msg_buffer[5], msg_buffer[4]);
        short acc_z  = concat_bytes(msg_buffer[7], msg_buffer[6]);

        short distance = concat_bytes(msg_buffer[9], msg_buffer[8]);
	char speed    = msg_buffer[10];

	// queue_message("Received bytes, distance: " + std::to_string(msg_buffer[9]) +  ", " + std::to_string(msg_buffer[8]));

        result.acc_x = acc_x;
        result.acc_y = acc_y;
        result.acc_x = acc_z;

        result.dist = distance;
        result.speed = speed;

	// queue_message("Received distance: " + std::to_string(distance) + ", speed: " + std::to_string(speed) + ", acc_x: " + std::to_string(acc_x) + ", acc_y:" + std::to_string(acc_y) + ", acc_z " + std::to_string(acc_z));

        sensor_data* sd = (sensor_data*) data_registry::get_instance().acquire_data(SENSOR_DATA_ID);
        *sd = result;
        data_registry::get_instance().release_data(SENSOR_DATA_ID);

        telemetrics_data* td = (telemetrics_data*) data_registry::get_instance().acquire_data(TELEMETRICS_DATA_ID);
        td->curr_speed = static_cast<double>(speed);
        data_registry::get_instance().release_data(TELEMETRICS_DATA_ID);
    
        return;
    }

    queue_message("Failed communication with sensor module 5 times, skipping.");
}

std::string print_buffer(uint8_t* buffer, int size) {
    std::stringstream s;
    for (int i = 0; i < size; i++) {
        s << std::hex << (int) buffer[i] << " ";
    }
    return s.str();
}

void send_control_data() {

    unsigned char start_buffer[SPI_CONTROL_INIT_MSG_SIZE];
    unsigned char msg_buffer[SPI_CONTROL_DATA_MSG_SIZE];
    unsigned char ans_buffer[SPI_CONTROL_FINISH_MSG_SIZE];

    regulator_out_data data;

    regulator_out_data* registry_entry = 
        (regulator_out_data*) data_registry::get_instance().acquire_data(REGULATOR_OUT_DATA_ID);
    
    data = *registry_entry;
    data_registry::get_instance().release_data(REGULATOR_OUT_DATA_ID);

    int fails = 0;
    while (fails < SPI_FAIL_COUNT) {

        start_buffer[0] = SPI_START_CONTROL;

        // If wiring pi library is not present, return
        #ifndef __WIRING_PI_H__ 
            return;
        #endif

        spi_write(SPI_CONTROL, start_buffer, SPI_CONTROL_INIT_MSG_SIZE);

        if (start_buffer[0] != SPI_ACK) {
            queue_message("Failed initialising communication with steering module. Retrying in 1 ms");
            std::this_thread::sleep_for(std::chrono::milliseconds(SPI_FAILED_WAIT_MS));
            fails++;
            continue;
        }

        msg_buffer[0] = static_cast<int8_t>(data.speed);
        msg_buffer[1] = static_cast<int8_t>(data.angle);
        msg_buffer[2] = SPI_NAN;

        unsigned char expected_checkbyte = calc_checkbyte(msg_buffer, SPI_CONTROL_DATA_MSG_SIZE - 1);

        // Write the data
        spi_write(SPI_CONTROL, msg_buffer, SPI_CONTROL_DATA_MSG_SIZE);

        unsigned char answer = SPI_FINISHED;

        unsigned char checkbyte = msg_buffer[2];

        // Test for checkbyte, if the checkbyte is wrong, drop the data and log an error.
        if (checkbyte != expected_checkbyte) {
            queue_message(
                "Error: Control data check byte validation failed. Got " + 
                std::to_string(checkbyte) + 
                " expected " +
                std::to_string(expected_checkbyte) +
                ". Retrying communication.");
            answer = SPI_RESTART;
        }  

        ans_buffer[0] = answer;

        // Write answer
        spi_write(SPI_CONTROL, ans_buffer, SPI_CONTROL_FINISH_MSG_SIZE);

        if (answer == SPI_FINISHED) return;

        fails++;
    }

    queue_message("Failed communication with steering module 5 times, skipping.");
}

void io_thread_main(const std::atomic_bool& running) {

    update_controller upd_controller;

    // Set up SPI busses
    #ifdef __WIRING_PI_H__ 
        queue_message("Setting up SPI channels.");
        wiringPiSetup();
        wiringPiSPISetup(SPI_CHANNEL, SPI_FREQ);
        pinMode(SPI_CONTROL, OUTPUT);
        pinMode(SPI_SENSOR, OUTPUT);

        // Slave select is active low, so set pins to high
        digitalWrite(SPI_CONTROL, 1);
        digitalWrite(SPI_SENSOR, 1);
    #endif

    while (running) {

        upd_controller.start();

        acquire_sensor_data();
        send_control_data();

        upd_controller.wait();
    }
}

void spi_write(int slave, unsigned char* buffer, int size) {

    #ifdef __WIRING_PI_H__
        // queue_message("Transmitted " + print_buffer((uint8_t*) buffer, size));
        activate_slave(slave);
        wiringPiSPIDataRW(SPI_CHANNEL, buffer, size);
        deactivate_slave(slave);
        // queue_message("Received " + print_buffer((uint8_t*) buffer, size));
    #endif
}

char calc_checkbyte(unsigned char* buffer, int size) {
    
    char acc = buffer[0];
    for (int i = 1; i < size; i++) {
        acc ^= buffer[i];
    }

    return acc;
}

void set_slave(int slave, int val) {

    #ifdef __WIRING_PI_H__ 
        digitalWrite(slave, val);
    #endif
}

void activate_slave(int slave) {
    set_slave(slave, 0);
}

void deactivate_slave(int slave) {
    set_slave(slave, 1);
}

bool test_checkbyte(unsigned char* buffer, int size, char checkbyte) {

    char true_checkbyte = calc_checkbyte(buffer, size);
    return true_checkbyte == checkbyte;
}

short concat_bytes(char b1, char b2) {
    return (b1 << 8) + b2;
}
