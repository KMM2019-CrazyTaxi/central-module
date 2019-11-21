#include "image_util.hpp"

#ifdef QPU_MODE

#include "QPULib.h"
#include "qpu_cursor.hpp"

#endif

#include <vector>

using namespace std;

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

void write_image(const uint8_t* image, ostream& output,
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

#ifdef QPU_MODE

void sobel_qpu(Ptr<Float> grid, Ptr<Float> gridOut, Int width, Int height)
{
    Cursor row[3];
    grid = grid + width*me() + index();

    // Skip first row of output grid
    gridOut = gridOut + width;

    For (Int y = me(), y < height, y=y+numQPUs())
        // Point p to the output row
        Ptr<Float> p = gridOut + y*width;

    // Initilaise three cursors for the three input rows
    for (int i = 0; i < 3; i++) row[i].init(grid + i*width);
    for (int i = 0; i < 3; i++) row[i].prime();

    // Compute one output row
    For (Int x = 0, x < width, x=x+16)

        for (int i = 0; i < 3; i++) row[i].advance();

    Float left[3], right[3];
    for (int i = 0; i < 3; i++) {
        row[i].shiftLeft(right[i]);
        row[i].shiftRight(left[i]);
    }

    Float sum = left[0] - right[0] +
        2*left[1] - 2*right[1] +
        left[2] - right[2];

    store(sum, p);
    p = p + 16;

    End

        // Cursors are finished for this row
        for (int i = 0; i < 3; i++) row[i].finish();

    // Move to the next input rows
    grid = grid + width*numQPUs();
    End
        }

void sobel(SharedArray<float>& image, SharedArray<float>& result, 
           const uint32_t width, const uint32_t height) {

    auto qpu_routine = compile(sobel_qpu);
    qpu_routine.setNumQPUs(4);
    qpu_routine(&image, &result, width, height);

}

void rgb2gray_qpu(Ptr<Float> colour, Ptr<Float> gray, Int width, Int height) {
    Cursor green;
    colour = colour + 3*width*me() + 3*index();

    For (Int y = me(), y < height, y=y+numQPUs())
        // Point p to the output row
        Ptr<Float> p = gray + y*width;

    // Initilaise three cursors for the three input rows
    green.init(colour);
    green.prime();

    // Compute one output row
    For (Int x = 0, x < width, x=x+16)
        // Advance three times to get to next red pixel.
        green.advance();
    green.advance();

    Float red, blue;
    green.shiftLeft(red);
    green.shiftRight(blue);

    Float val = red + green.current + blue;

    store(val, p);
    p = p + 16;
    green.advance();

    End
        green.finish();

    // Move to the next input rows
    gray = gray + width*numQPUs();
    End
}

void rgb2gray(SharedArray<float>& image, SharedArray<float>& result,
              const uint32_t width, const uint32_t height) {
    auto qpu_routine = compile(rgb2gray_qpu);
    qpu_routine.setNumQPUs(4);
    qpu_routine(&image, &result, width, height);
}

#else

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

void rgb2gray(const uint8_t* image, uint8_t* gray, const uint32_t width, const uint32_t height)
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

#endif

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

