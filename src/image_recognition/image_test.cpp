#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <string>

#include "image_util.hpp"

using namespace std;

const uint32_t WIDTH{ 320 };
const uint32_t HEIGHT{ 240 };
const uint32_t COLORS{ 3 };
const uint32_t SIZE_RGB{ WIDTH * HEIGHT * COLORS };
const uint32_t SIZE_GRAY{ WIDTH * HEIGHT };

const string FILE_NAMES[8] {"1", "2", "3", "4",
	"5", "6", "7", "8"};

int main() {
    for (int file{}; file < 8; ++file) {
	ifstream input{ FILE_NAMES[file] + ".ppm", ios::binary };
	uint8_t* image = new uint8_t[SIZE_RGB];
	uint8_t* gray = new uint8_t[SIZE_GRAY];
	uint8_t* blur = new uint8_t[SIZE_GRAY];
	uint8_t* edge = new uint8_t[SIZE_GRAY];
	uint8_t* marked = new uint8_t[SIZE_RGB];

	vector<int> left, right;

	read_image(image, input);
	memcpy(marked, image, SIZE_RGB);
    
	rgb2gray(image, gray, WIDTH, HEIGHT);
//    gauss(gray, blur, WIDTH, HEIGHT);
	sobel(gray, edge, WIDTH, HEIGHT);
	get_max_edge(edge, left, right, WIDTH, HEIGHT);
	mark_edges(edge, marked, left, right, WIDTH, HEIGHT);

	ofstream output{ FILE_NAMES[file] + "_gray.ppm", ios::binary };
	write_image(gray, output, WIDTH, HEIGHT, 1);
	output.close();

	output.open( FILE_NAMES[file] + "_blur.ppm", ios::binary);
	write_image(blur, output, WIDTH, HEIGHT, 1);
	output.close();

	output.open( FILE_NAMES[file] + "_edge.ppm", ios::binary);
	write_image(edge, output, WIDTH, HEIGHT, 1);
	output.close();

	output.open( FILE_NAMES[file] + "_marked.ppm", ios::binary);
	write_image(marked, output, WIDTH, HEIGHT, 3);
	output.close();

	delete image;
	delete gray;
	delete blur;
	delete edge;
    }
    return 0;
}
