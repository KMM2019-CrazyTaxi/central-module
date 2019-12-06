#pragma once

#include <cmath>
#include <iostream>
#include <vector>

#include "image_recognition_constants.hpp"

/*
 * Read an image saved in .ppm format from file.
 *
 * image: A pointer to buffer large enough to contain the image.
 * input: An opened istream with the image data.
 */
void read_image(uint8_t* image, std::istream& input);

/*
 * Writes an image to file in .ppm format.
 *
 * image: A buffer to the image data to write.
 * output: An opened ostream to write the file to.
 * width: The image width in pixels.
 * height: The image height in pixels.
 * type: RGB or GRAY matching the image color space.
 */
void write_image(const uint8_t* image, std::ostream& output,
		 const uint32_t width, const uint32_t height, 
                 const IMAGE_TYPE type = RGB);

/*
 * Converts an rgb image to approximate grayscale. The grayscale value of each
 * pixel is the sum the three colour channels divided by two, capping at 255.
 * This means that all bright pixels in the input image will be capped at white
 * colour in the grayscale image, smoothing out glare.
 *
 * image: A buffer containing an rgb image.
 * gray: A buffer to write the grayscale image to. Must be preallocated and large
 *       enough to contain the converted image.
 * width: The image width in pixels.
 * height: The image height in pixels.
 */
void rgb2gray(const uint8_t* image, uint8_t* gray, 
              const uint32_t width, const uint32_t height);

/*
 * Applies a sobel operator in the x dimension to an image.
 *
 * image: A buffer containing a grayscale image.
 * result: A preallocated buffer to write the edge information to.
 * width: The image width in pixels.
 * height: The image height in pixels.
 */
void sobelx(const uint8_t* image, uint8_t* result,
            const uint32_t width, const uint32_t height);

/*
 * Applies a sobel oeprator in the y dimension to an image.
 * 
 * image: A buffer containing a grayscale image.
 * result: A preallocated buffer to write the edge information to.
 * width: The image width in pixels.
 * height: The image height in pixels.
 */
void sobely(const uint8_t* image, uint8_t* result,
            const uint32_t width, const uint32_t height);

/*
 * Finds the most prominent side edge in an image. The search in each row starts
 * at a location close to the previous edge, defined by the old_edge vector and
 * the PIXEL_RANGE_FROM_OLD_EDGE constant. If a new maxima is detected that is
 * stronger than the previous by a factor determined by RELATIVE_DGE_STRENGTH_THRESHOLD,
 * this new maxima is considered the most prominent edge going forward.
 *
 * edgex_image: The image produced by a sobel operator in the x dimension.
 * old_edge: Vector where the edge information is to be saved. When calling this function,
 *           the vector should contain edge information from the previous scan. These values 
 *           are replaced with the new pixel index with the most prominent edge.
 * width: The image width in pixels.
 * height: The image height in pixels.
 */
void get_max_side_edge(const uint8_t* edgex_image, std::vector<uint32_t>& old_edge,
                       const uint32_t width, const uint32_t height);

void get_max_front_edge(const uint8_t* edgey_image, std::vector<uint32_t>& old_edge,
                        const uint32_t width, const uint32_t height);

double get_distance_to_side(const uint8_t* edge_image, std::vector<uint32_t>& best,
                            const uint32_t start_row, const uint32_t end_row,
                            const uint32_t width, const uint32_t height,
			    const double default_edge);

double get_distance_to_stop(const uint8_t* edge_image, std::vector<uint32_t>& best,
                            const uint32_t start_column, const uint32_t end_column,
                            const uint32_t width, const uint32_t height);

void mark_selected_edges(uint8_t* marked, 
                         const double left, const double right, const double front,
                         const uint32_t start_row, const uint32_t end_row,
                         const uint32_t start_column, const uint32_t end_column,
                         const uint32_t width, const uint32_t height);

/*
 * Marks the most priminent edges in an image. Left, right and front edges are marked
 * in red, green and blue, respectively. Detected edges must be above the threshols value
 * set by EDGE_STRENGTH_THRESHOLD.
 *
 * marked: An rgb image to mark the edges in.
 * left, right, front: The vectors with edge information produced by get_max_edge.
 * width: The image width in pixels.
 * height: The image height in pixels.
 */
void mark_all_edges(uint8_t* marked,
                    const std::vector<uint32_t>& left, const std::vector<uint32_t>& right,
                    const std::vector<uint32_t>& front,
                    const uint32_t width, const uint32_t height);
