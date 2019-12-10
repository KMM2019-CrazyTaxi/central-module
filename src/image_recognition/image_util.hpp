#pragma once

#include <opencv2/opencv.hpp>

#include <cmath>
#include <iostream>
#include <vector>

#include "image_recognition_constants.hpp"

cv::Mat to_mat(const uint8_t* rgb_image,
               const uint32_t width, const uint32_t height, const IMAGE_TYPE type);

/*
 * Read an image saved in .ppm format from file.
 *
 * image: A pointer to buffer large enough to contain the image.
 * input: An opened istream with the image data.
 */
void read_image(uint8_t* image, std::istream& input);

/*
 * Writes an image to file.
 *
 * image: A buffer to the image data to write.
 * output: The file name to write to.
 * width: The image width in pixels.
 * height: The image height in pixels.
 * type: RGB or GRAY matching the image color space.
 */
void write_image(const uint8_t* image, const std::string& file_name,
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

void rgb2hsv(const uint8_t* image, uint8_t* hsv,
             const uint32_t width, const uint32_t height);

/*
 * Applies a sobel operator in the x dimension to an image.
 *
 * image: A buffer containing a grayscale image.
 * result: A preallocated buffer to write the edge information to.
 * width: The image width in pixels.
 * height: The image height in pixels.
 */
void sobelx(const uint8_t* rgb_image, const uint8_t* gray_image, uint8_t* result,
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
 * Finds the most prominent left, right and forward edges respectively in an
 * image. For the left and right edges, the function starts in the center of
 * the image and tranverses towards the edge, keeping track of local edge maxima.
 * If a new maxima is detected that is stronger than the previous by a factor
 * determined by RELATIVE_DGE_STRENGTH_THRESHOLD, this new maxima is considered
 * the most prominent edge going forward. The forward edge is calculated in a
 * similar way, but starting at the bottom of the image instead of the center.
 * This process is repeated for each row/column.
 *
 * edgex_image: The image produced by a sobel operator in the x dimension.
 * edgey_image: The image produced by a sobel operator in the y dimension.
 * left, right, front: Vectors where the edge information is to be saved. The
 *                     pixel index with the most prominent edge is saved.
 * width: The image width in pixels.
 * height: The image height in pixels.
 */
void get_max_edge(const uint8_t* edgex_image, const uint8_t* edgey_image,
                  std::vector<uint32_t>& left, std::vector<uint32_t>& right,
                  std::vector<uint32_t>& front,
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
 * edgex_image, edgey_image: The images produced by solbex and sobely, respectively.
 * marked: An rgb image to mark the edges in.
 * left, right, front: The vectors with edge information produced by get_max_edge.
 * width: The image width in pixels.
 * height: The image height in pixels.
 */
void mark_all_edges(const uint8_t* edgex_image, const uint8_t* edgey_image, uint8_t* marked,
                    const std::vector<uint32_t>& left, const std::vector<uint32_t>& right,
                    const std::vector<uint32_t>& front,
                    const uint32_t width, const uint32_t height);
