#ifndef CM_DEFS_H
#define CM_DEFS_H

#define IO_THREAD_NAME "IO Thread"
#define IR_THREAD_NAME "IR Thread"
#define NET_THREAD_NAME "Network Thread"
#define PID_THREAD_NAME "Regulator Thread"
#define MAIN_THREAD_NAME "Main Thread"

#define PID_K_SPEED_INITIAL 1
#define PID_K_TURN_INITIAL 1
#define PID_TD_TURN_INITIAL 0

#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0x4000
#endif

#endif
