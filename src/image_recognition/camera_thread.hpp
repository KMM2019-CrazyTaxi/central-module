#ifndef CM_CAMERA_THREAD_H
#define CM_CAMERA_THREAD_H

#include <atomic>
#include "double_buffer.hpp"

#define IMAGE_SIZE 1024

void camera_thread_main(const std::atomic_bool& running, double_buffer& image_buffer);

#endif