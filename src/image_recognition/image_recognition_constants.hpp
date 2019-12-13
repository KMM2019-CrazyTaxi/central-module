#pragma once

#include <string>

// Number of pixels per row in an image.
const uint32_t IMAGE_WIDTH{ 320 };

// Number of rows in an image.
const uint32_t IMAGE_HEIGHT{ 240 };

// Bytes per grayscale image.
const uint32_t IMAGE_SIZE_GRAY{ IMAGE_WIDTH * IMAGE_HEIGHT };

// Bytes per rgb image.
const uint32_t IMAGE_SIZE_RGB{ IMAGE_SIZE_GRAY * 3 };

// Lowest edge strength that is considered an edge.
const uint32_t EDGE_STRENGTH_THRESHOLD{ 64 };

/*
 * The side edge detection prefers edges that are closer to the center of
 * the image. This constant determines the lowest relative strength for an
 * outer edge to be prefered over an inner edge.
 */
const double RELATIVE_EDGE_STRENGTH_THRESHOLD{ 1 };

// Number of available test images.
const uint32_t N_TEST_IMAGES{ 8 };

// Path of test images relative to the project root.
const std::string TEST_IMAGE_PATH{ "./src/image_recognition/res/" };

// Names of available test images.
const std::string TEST_IMAGES[N_TEST_IMAGES] = {"1.ppm", "2.ppm", "3.ppm", "4.ppm",
                                                "5.ppm", "6.ppm", "7.ppm", "8.ppm"};

// If true, each processed image is save to a file for debug purposes.
const bool OUTPUT_MARKED_IMAGE_TO_FILE{ true };

// Frame taken by the camera each second.
const uint32_t CAMERA_FPS{ 60};

// Distance from car where the side edges are detected.
const uint32_t BOUND_DISTANCE_1_PIXEL{ 75 };
const uint32_t BOUND_DISTANCE_2_PIXEL{ 200 };

/*
The start and end distances in pixels from the bottom of the image where ir should
look for stop lines. They are assumed to be in the range [0, IMAGE_HEIGHT). Note
that the sobel convolutions create noise close the the image bounds, so these constants
should not be set less than 5 pixels from range end points.
*/
const uint32_t STOP_LINE_START_DISTANCE{ 50 };
const uint32_t STOP_LINE_END_DISTANCE{ 200 };

const uint32_t STOP_LINE_FACTOR{ 5 };

// The number of pixels to used to calculate average edge location.
const uint32_t EDGE_AVG_PIXELS{ 20 };

const double IMAGE_WIDTH_AT_BOUND_DISTANCE_1{ 40 };
const double IMAGE_WIDTH_AT_BOUND_DISTANCE_2{ 60 };

const double CM_PER_PIXEL_AT_BOUND_DISTANCE_1{ IMAGE_WIDTH_AT_BOUND_DISTANCE_1 / IMAGE_WIDTH };
const double CM_PER_PIXEL_AT_BOUND_DISTANCE_2{ IMAGE_WIDTH_AT_BOUND_DISTANCE_2 / IMAGE_WIDTH };

enum IMAGE_TYPE{ RGB, GRAY };
