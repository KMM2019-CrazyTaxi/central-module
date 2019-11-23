#include "image_util.hpp"

#ifdef QPU

#include "QPULib.h"
#include "qpu_cursor.hpp"

#endif

#include <vector>

void read_image(uint8_t* image, std::istream& input) {
    uint32_t width, height, scale, size;
    std::string format;

    input >> format >> width >> height >> scale;
    if (format == "P5") {
        size = width * height;
    } else {
	size = width * height * 3;
    }
    input.read((char*) image, size);
}

void write_image(const uint8_t* image, std::ostream& output,
		 const uint32_t width, const uint32_t height, const IMAGE_TYPE type) {
    uint32_t size{};
    std::string format{};

    switch (type) {
    case GRAY:
        size = width * height;
	format = "P5\n";
        break;
    case RGB:
        size = width * height * 3;
	format = "P6\n";
        break;
    }

    output << format << width << " " << height << " 255\n";
    output.write((char*) image, size);
}

#ifdef QPU

void sobel_qpu(Ptr<Int> grid, Ptr<Int> gridOut, Int width, Int height)
{
    Cursor row[3];
    grid = grid + width*me() + index();

    // Skip first row of output grid
    gridOut = gridOut + width;

    For (Int y = me(), y < height, y=y+numQPUs())
        // Point p to the output row
        Ptr<Int> p = gridOut + y*width;

    // Initilaise three cursors for the three input rows
    for (int i = 0; i < 3; i++) row[i].init(grid + i*width);
    for (int i = 0; i < 3; i++) row[i].prime();

    // Compute one output row
    For (Int x = 0, x < width, x=x+16)

        for (int i = 0; i < 3; i++) row[i].advance();

    Int left[3], right[3];
    for (int i = 0; i < 3; i++) {
        row[i].shiftLeft(right[i]);
        row[i].shiftRight(left[i]);
    }

    Int sum = left[0] - right[0] +
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

void rgb2gray_qpu(Ptr<Int> colour, Ptr<Int> gray, Int width, Int height) {
    Cursor green;
    colour = colour + 3*width*me() + 3*index();

    For (Int y = me(), y < height, y=y+numQPUs())
        // Point p to the output row
        Ptr<Int> p = gray + y*width;

    // Initilaise three cursors for the three input rows
    green.init(colour);
    green.prime();

    // Compute one output row
    For (Int x = 0, x < width, x=x+16)
        // Advance three times to get to next red pixel.
        green.advance();
    green.advance();

    Int red, blue;
    green.shiftLeft(red);
    green.shiftRight(blue);

    Int val = red + green.current + blue;

    store(val, p);
    p = p + 16;
    green.advance();

    End
        green.finish();

    // Move to the next input rows
    gray = gray + width*numQPUs();
    End
}

#else

void sobelx(const uint8_t* image, uint8_t* result, 
            const uint32_t width, const uint32_t height) {
    const uint32_t size{ width * height };
    for (uint32_t pos{1}; pos < size - 1; ++pos) {
        int32_t sum = image[pos - 1] - image[pos + 1];
        result[pos] = sum < 0 ? 0 : sum;
    }
}

void sobely(const uint8_t* image, uint8_t* result, 
            const uint32_t width, const uint32_t height) {
    const uint32_t size{ width * height };
    for (uint32_t pos{width}; pos < size - width; ++pos) {
        int32_t sum = image[pos - width] - image[pos + width];
        result[pos] = sum < 0 ? 0 : sum;
    }
}

void rgb2gray(const uint8_t* image, uint8_t* gray, 
              const uint32_t width, const uint32_t height)
{
    /*
     * Approximates true rgb2gray conversion by summing colour channels and
     * dividing by 2. This is much faster that true rgb2gray conversion using
     * scalar products, and as an added bonus tends to smooth bright colours
     * to an even white with few edges.
     */
    const uint32_t size{ width * height };
    for (uint32_t pos{}; pos < size; ++pos) {
        uint32_t sum{};
        for (uint32_t c{}; c < 3; ++c) {
            sum += *image++;
        }
        sum >>= 1;
        *(gray++) = sum >= 255 ? 255 : sum;
    }
}

#endif

void gauss(const uint8_t* image, uint8_t* result,
           const uint32_t width, const uint32_t height)
{
    int kernel[5][5] = {{1, 4, 6, 4, 1},
			{4, 16, 24, 1, 4},
			{6, 24, 36, 24, 6},
			{4, 16, 24, 16, 4},
			{1, 4, 6, 4, 1}};
    for (uint32_t h{2}; h < height - 2; ++h) {
	for (uint32_t w{2}; w < width - 2; ++w) {
	    uint32_t sum{};
	    for (uint32_t i{}; i < 5; ++i) {
		for (uint32_t j{}; j < 5; ++j) {
		    sum += kernel[i][j] * image[(h + i - 2) * width + w + j - 2];
		}
	    }
	    result[h * width + w] = (sum >> 8);
	}
    }
}

void get_max_edge(const uint8_t* image, 
                  std::vector<uint32_t>& left, std::vector<uint32_t>& right,
		  const uint32_t width, const uint32_t height) {
    for (int row{}; row < height; ++row) {
        const uint32_t row_start{ row * width };
        const uint32_t row_middle{ width >> 1 };

	uint8_t strongest_strength{ image[row_start + row_middle] };
	uint32_t strongest_edge_pixel{ row_middle };
        double min_stronger_edge_strength{ 
            strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD };
	for (uint32_t column{ row_middle }; column >= 1; --column) {
	    if (image[row_start + column] >= min_stronger_edge_strength) {
		strongest_strength = image[row_start + column];
		strongest_edge_pixel = column;
                min_stronger_edge_strength =
                    strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD;
	    }
	}
	left.push_back(strongest_edge_pixel);

	strongest_strength = image[row_start + row_middle];
	strongest_edge_pixel = row_middle;
        min_stronger_edge_strength = 
            strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD;
	for (uint32_t column{ row_middle }; column < width; ++column) {
	    if (image[row_start + column] >= min_stronger_edge_strength) {
		strongest_strength = image[row_start + column];
		strongest_edge_pixel = column;
                min_stronger_edge_strength =
                    strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD;
	    }
	}
	right.push_back(strongest_edge_pixel);
    }
}

void mark_edges(const uint8_t* edge, uint8_t* marked,
		const std::vector<uint32_t>& left, const std::vector<uint32_t>& right,
                const uint32_t width, const uint32_t height)
{
    for (uint32_t row{}; row < height; ++row) {
        const uint32_t row_start{ row * width };

	uint32_t strongest_pixel{ left[row] };
	if (edge[row_start + strongest_pixel] > EDGE_STRENGTH_THRESHOLD) {
            const uint32_t marked_pixel_index{ (row_start + strongest_pixel) * 3 };
	    marked[marked_pixel_index] = 255;
	    marked[marked_pixel_index + 1] = 0;
	    marked[marked_pixel_index + 2] = 0;
	}

    	strongest_pixel = right[row];
	if (edge[row_start + strongest_pixel] > EDGE_STRENGTH_THRESHOLD) {
            const uint32_t marked_pixel_index{ (row_start + strongest_pixel) * 3 };
	    marked[marked_pixel_index] = 0;
	    marked[marked_pixel_index + 1] = 255;
	    marked[marked_pixel_index + 2] = 0;
	}
    }
}

