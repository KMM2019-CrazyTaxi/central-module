#ifndef CM_MESSAGE_QUEUE_H
#define CM_MESSAGE_QUEUE_H

#include <thread>
#include <string>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>


/**
 * A single entry in the message queue
 */ 
struct message_queue_entry {
    
    // ID of the thread that queued this message
    std::thread::id thread_id;

    // The message to be printed
    std::string message;

    // The timepoint at which the message was inserted
    std::chrono::system_clock::time_point time_of_arrival;
};

/**
 * The shared queue that keeps track of messages to be printed
 */
struct message_queue {
    
    // The queue that contains the message entries
    std::queue<message_queue_entry> queue;
    
    // Lock of the message queue
    std::mutex queue_lock;

    // Condition variable to notify logging thread
    std::condition_variable queue_condition;
};

#endif