#ifndef CM_REGISTRY_ENTRIES_H
#define CM_REGISTRY_ENTRIES_H


#define SENSOR_DATA_ID "sensor_data"
#define CONTROL_CHANGE_DATA_ID "control_change_data"

struct sensor_data {
    float distance;
    float acceleration;
};

struct control_change_data {
    char speed;
    char angle;
};

#endif