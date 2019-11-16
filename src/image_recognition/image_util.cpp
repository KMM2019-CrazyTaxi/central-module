#include "image_util.hpp"
#include <vector>

using namespace std;

void rgb2gray(uint8_t* image, uint8_t* gray, const uint32_t width, const uint32_t height)
{
    for (uint32_t pixel{}; pixel < width*height; ++pixel) {
	uint32_t sum{};
	for (uint32_t c{}; c < 3; ++c) {
	    sum += *image * *image;
	    ++image;
	}
	uint32_t val = sum / sqrt(195075);
	*(gray++) = val >= 128 ? 255 : val * 2;
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

void sobel(uint8_t* image, uint8_t* result, const int32_t width, const int32_t height) {
    int kernel[3][3] = {{1, 0, -1},
			{2, 0, -2},
			{1, 0, -1}};
    for (int32_t h{1}; h < height - 1; ++h) {
	for (int32_t w{1}; w < width - 1; ++w) {
	    int sum{};
	    for (int32_t i{}; i < 3; ++i) {
		for (int32_t j{}; j < 3; ++j) {
		    sum += kernel[i][j] * image[(h + i - 1) * width + w + j - 1];
		}
	    }
	    result[h * width + w] = sum < 0 ? 0 : sum;
	}
    }
}

void get_max_edge(uint8_t* image, vector<int>& left, vector<int>& right,
		  const int32_t width, const int32_t height) {
    for (int row{}; row < height; ++row) {
	
	uint8_t strongest_strength{ image[row * width + width / 2] };
	int strongest_edge{ width / 2 };
	for (int col{ width / 2 }; col >= 3; --col) {
	    if (image[row * width + col] >= 2*strongest_strength) {
		strongest_strength = image[row * width + col];
		strongest_edge = col;
	    }
	}
	left.push_back(strongest_edge);

	strongest_strength = image[row * width + width / 2];
	strongest_edge = width / 2;
	for (int col{ width / 2 }; col < width - 3; ++col) {
	    if (image[row * width + col] >= 2*strongest_strength) {
		strongest_strength = image[row * width + col];
		strongest_edge = col;
	    }
	}
	right.push_back(strongest_edge);
    }
}

void mark_edges(uint8_t* edge, uint8_t* marked,
		vector<int>& left, vector<int>& right, const int32_t width, const int32_t height)
{
    for (int h{}; h < height; ++h) {
	int w{ left[h] };
	if (edge[h*width + w] > 128) {
	    marked[h*width*3 + w*3] = 255;
	    marked[h*width*3 + w*3 + 1] = 0;
	    marked[h*width*3 + w*3 + 2] = 0;
	}

    	w = right[h];
	if (edge[h*width + w] > 128) {
	    marked[h*width*3 + w*3] = 0;
	    marked[h*width*3 + w*3 + 1] = 255;
	    marked[h*width*3 + w*3 + 2] = 0;
	}
    }
}
