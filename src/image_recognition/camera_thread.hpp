#ifndef CM_CAMERA_THREAD_H
#define CM_CAMERA_THREAD_H

#include <atomic>
#include "double_buffer.hpp"
#include "image_recognition_constants.hpp"

void camera_thread_main(const std::atomic_bool& running, double_buffer& image_buffer);

#endif
