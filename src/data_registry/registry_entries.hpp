#ifndef CM_REGISTRY_ENTRIES_H
#define CM_REGISTRY_ENTRIES_H


#define SENSOR_DATA_ID          1
#define CONTROL_CHANGE_DATA_ID  2

struct sensor_data {
    int16_t acc_x;
    int16_t acc_y;
    int16_t acc_z;
    uint8_t dist;
    uint8_t speed;
};

struct control_change_data {
    char speed;
    char angle;
};

#endif