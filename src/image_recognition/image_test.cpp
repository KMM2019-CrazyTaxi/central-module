#include <iostream>
#include <fstream>
#include <string>

#include "image_util.hpp"

using namespace std;

const uint32_t WIDTH{ 320 };
const uint32_t HEIGHT{ 240 };
const uint32_t COLORS{ 3 };
const uint32_t SIZE_RGB{ WIDTH * HEIGHT * COLORS };
const uint32_t SIZE_GRAY{ WIDTH * HEIGHT };

int main() {
    ifstream input{ "1.ppm", ios::binary };
    uint8_t* image = new uint8_t[SIZE_RGB];
    read_image(image, input);
    
    uint8_t* gray = new uint8_t[SIZE_GRAY];
    rgb2gray(image, gray, WIDTH, HEIGHT);
    uint8_t* blur = new uint8_t[SIZE_GRAY];
    gauss(gray, blur, WIDTH, HEIGHT);

    ofstream output{ "1_gray.ppm", ios::binary };
    write_image(gray, output, WIDTH, HEIGHT, 1);
    output.close();

    output.open("1_blur.ppm", ios::binary);
    write_image(blur, output, WIDTH, HEIGHT, 1);
    output.close();

    delete image;
    delete gray;
    delete blur;
    return 0;
}
