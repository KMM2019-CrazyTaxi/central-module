using namespace std;

#include "image_util.hpp"

void rgb2gray(uint8_t* image, uint8_t* gray, const uint32_t width, const uint32_t height)
{
    for (uint32_t pixel{}; pixel < width*height; ++pixel) {
	uint32_t sum{};
	for (uint32_t c{}; c < 3; ++c) {
	    sum += *(image++);
	}
	*(gray++) = sum / 3;
    }
}

void read_image(uint8_t* image, istream& input) {
    uint32_t width, height, scale, size;
    string format;

    input >> format >> width >> height >> scale;
    if (format == "P5") {
	size = width * height;
    } else {
	size = width * height * 3;
    }
    input.read((char*) image, size);
}

void write_image(uint8_t* image, ostream& output,
		 const uint32_t width, const uint32_t height, const uint32_t colours) {
    uint32_t size = width * height * colours;
    string format{};
    if (colours == 1) {
	format = "P5\n";
    } else {
	format = "P6\n";
    }
    output << format << width << " " << height << " 255\n";
    output.write((char*) image, size);
}

void gauss(uint8_t* image, uint8_t* result, const int32_t width, const int32_t height)
{
    int kernel[5][5] = {{1, 4, 6, 4, 1},
			{4, 16, 24, 1, 4},
			{6, 24, 36, 24, 6},
			{4, 16, 24, 16, 4},
			{1, 4, 6, 4, 1}};
    for (int32_t h{2}; h < height - 2; ++h) {
	for (int32_t w{2}; w < width - 2; ++w) {
	    uint32_t sum{};
	    for (int32_t i{}; i < 5; ++i) {
		for (int32_t j{}; j < 5; ++j) {
		    sum += kernel[i][j] * image[(h + i - 2) * width + w + j - 2];
		}
	    }
	    result[h * width + w] = (sum >> 8);
	}
    }
}
