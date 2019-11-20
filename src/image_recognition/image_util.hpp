#pragma once

#include <cmath>
#include <iostream>
#include <vector>

void read_image(uint8_t* image, std::istream& input);
void write_image(const uint8_t* image, std::ostream& output,
		 const uint32_t width, const uint32_t height, const uint32_t colours);
void rgb2gray(const uint8_t* image, uint8_t* gray, const uint32_t width, const uint32_t height);
void gauss(uint8_t* image, uint8_t* result, const int32_t width, const int32_t height);
void sobel(uint8_t* image, uint8_t* result, const int32_t width, const int32_t height);
void get_max_edge(uint8_t* image, std::vector<int>& left, std::vector<int>& right,
		  const int32_t width, const int32_t height);
void mark_edges(uint8_t* edge, uint8_t* marked,
		std::vector<int>& left, std::vector<int>& right, const int32_t width, const int32_t height);
