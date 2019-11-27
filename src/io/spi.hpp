#ifndef CM_SPI_H
#define CM_SPI_H

#define SPI_NAN         0x00
#define SPI_ERROR       0x55
#define SPI_FINISHED    0x66
#define SPI_RESTART     0x99
#define SPI_ACK         0xEE
#define SPI_CONFIRM     0xCC

#define SPI_START_CONTROL	0xAA
#define SPI_START_SENSOR	0x11

#define SPI_CONTROL_INIT_MSG_SIZE       1
#define SPI_CONTROL_DATA_MSG_SIZE       3
#define SPI_CONTROL_FINISH_MSG_SIZE     1

#define SPI_SENSOR_INIT_MSG_SIZE        2
#define SPI_SENSOR_CONFIRM_MSG_SIZE     1
#define SPI_SENSOR_DATA_MSG_SIZE        12
#define SPI_SENSOR_FINISH_MSG_SIZE      1

#define SPI_FAILED_WAIT_MS 1

#define SPI_FREQ    1000000 / 32
#define SPI_CONTROL 0 // Physical pin 11
#define SPI_SENSOR  1 // Physical pin 12
#define SPI_CHANNEL 0

#define SPI_FAIL_COUNT 5

#endif
