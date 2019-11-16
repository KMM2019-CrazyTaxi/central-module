#include <iostream>

void read_image(uint8_t* image, std::istream& input);
void write_image(uint8_t* image, std::ostream& output,
		 const uint32_t width, const uint32_t height, const uint32_t colours);
void rgb2gray(uint8_t* image, uint8_t* gray, const uint32_t width, const uint32_t height);
void gauss(uint8_t* image, uint8_t* result, const int32_t width, const int32_t height);
