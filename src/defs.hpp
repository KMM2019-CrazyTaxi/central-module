#ifndef CM_DEFS_H
#define CM_DEFS_H

#define IO_THREAD_NAME "IO Thread"
#define IR_THREAD_NAME "IR Thread"
#define CAM_THREAD_NAME "Camera Thread"
#define NET_THREAD_NAME "Network Thread"
#define PID_THREAD_NAME "Regulator Thread"
#define MAIN_THREAD_NAME "Main Thread"

#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0x4000
#endif

#endif
