#ifndef CM_DEFS_H
#define CM_DEFS_H

#define IO_THREAD_NAME "IO Thread"
#define IR_THREAD_NAME "IR Thread"
#define NET_THREAD_NAME "Network Thread"
#define MAIN_THREAD_NAME "Main Thread"

#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0x4000
#endif

#endif