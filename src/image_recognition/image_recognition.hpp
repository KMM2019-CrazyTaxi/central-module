#ifndef CM_IMAGE_RECOGNITION_H
#define CM_IMAGE_RECOGNITION_H

#include <atomic>

/**
 * Main function ran by the image recognition thread
 */
void image_recognition_main(const std::atomic_bool& running);

#endif