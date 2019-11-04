
#include <fstream>
#include <mutex>
#include <queue>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "logging.hpp"
#include "message_queue.hpp"

// The message queue used
static message_queue messages;

// Atomic variable that keeps track of the size of the queue
static std::atomic_int queue_size = 0;

// Map that maps thread ids to a string representation
static std::unordered_map<std::thread::id, std::string> thread_name_map;

void logging_thread_main(const std::unordered_map<std::thread::id, std::string>& name_map, const std::atomic_bool& running) {

    thread_name_map = name_map;
    std::ofstream output_file(LOG_FILE_PATH);

    while (running || queue_size > 0) {
        print_message(output_file);
    }

    output_file.close();
}

void queue_message(std::string&& message) {

    message_queue_entry new_entry;

    new_entry.message = std::move(message);
    new_entry.thread_id = std::this_thread::get_id();
    new_entry.time_of_arrival = std::chrono::system_clock::now();

    // Acquire lock
    std::unique_lock lock(messages.queue_lock);

    messages.queue.push(std::move(new_entry));

    queue_size++;

    lock.unlock();
    messages.queue_condition.notify_one();
}

void print_message(std::ofstream& file) {

    std::unique_lock lock(messages.queue_lock);
    messages.queue_condition.wait(lock, []{ return messages.queue.size() > 0; });

    message_queue_entry& entry = messages.queue.front();

    std::stringstream string_builder;

    std::time_t t = std::chrono::system_clock::to_time_t(entry.time_of_arrival);

    char buffer[32];
    sprintf(buffer, "%.24s", std::ctime(&t));

    string_builder << "[ " << buffer << " ]";
    string_builder << "[ " << thread_name_map[entry.thread_id] << " ]";
    string_builder << " " << entry.message << std::endl;

    std::string message_to_print = string_builder.str();
    
    file << message_to_print;
    std::cout << message_to_print;

    queue_size--;

    messages.queue.pop();
    lock.unlock();
}
