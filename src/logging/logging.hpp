#ifndef CM_LOGGING_H
#define CM_LOGGING_H

#define LOG_FILE_PATH "central_module_log"

#include <thread>
#include <atomic>
#include <unordered_map>

/**
 * Main function ran by the logging thread
 * 
 * @param name_map Map that maps thread ids to a string name representation
 */
void logging_thread_main(const std::unordered_map<std::thread::id, std::string>& name_map, const std::atomic_bool& running);

/**
 * Queues the message to be printed by the logging thread.
 * 
 * @param message The message to be printed the standard output and the log file
 *                NOTE: This function is potentially destructive for the message,
 *                since it uses a move to speed up insertion into the message queue
 */
void queue_message(std::string&& message);

/**
 * Pops a message from the queue and prints it to the given file.
 * NOTE: This function should only be called by the logging thread to preserve thread-safety.
 * 
 * @param file The log file to print this message to
 */
void print_message(std::ofstream& file);

#endif