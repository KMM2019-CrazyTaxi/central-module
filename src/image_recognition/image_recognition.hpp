#ifndef CM_IMAGE_RECOGNITION_H
#define CM_IMAGE_RECOGNITION_H

#include <atomic>

#include "double_buffer.hpp"

/**
 * Main function ran by the image recognition thread
 */
void image_recognition_main(const std::atomic_bool& running, double_buffer& image_buffer);

#endif