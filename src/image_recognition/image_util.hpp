#pragma once

#include <cmath>
#include <iostream>
#include <vector>

#include "image_recognition_constants.hpp"

#ifdef QPU

#include "QPULib.h"

#endif

void read_image(uint8_t* image, std::istream& input);
void write_image(const uint8_t* image, std::ostream& output,
		 const uint32_t width, const uint32_t height, 
                 const IMAGE_TYPE type = RGB);

#ifdef QPU

void rgb2gray_qpu(Ptr<Int> image, Ptr<Int> gray, Int width, Int height);
void sobel_qpu(Ptr<Int> image, Ptr<Int> result, Int width, Int height);

#else

void rgb2gray(const uint8_t* image, uint8_t* gray, 
              const uint32_t width, const uint32_t height);
void sobelx(const uint8_t* image, uint8_t* result,
            const uint32_t width, const uint32_t height);

#endif

void gauss(const uint8_t* image, uint8_t* result, 
           const uint32_t width, const uint32_t height);
void get_max_edge(const uint8_t* image, 
                  std::vector<uint32_t>& left, std::vector<uint32_t>& right,
		  const uint32_t width, const uint32_t height);
void mark_edges(const uint8_t* edge, uint8_t* marked,
		const std::vector<uint32_t>& left, const std::vector<uint32_t>& right, 
                const uint32_t width, const uint32_t height);
