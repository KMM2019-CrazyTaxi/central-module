#include "image_util.hpp"

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

void sobelx(const uint8_t* image, uint8_t* result, 
            const uint32_t width, const uint32_t height) {
    const uint32_t size{ width * height };
    for (uint32_t pos{1}; pos < size - 1; ++pos) {
        int32_t sum = image[pos - 1] - image[pos + 1];
        result[pos] = sum < 0 ? 0 : (sum > 255 ? 255 : sum);
    }
}

void sobely(const uint8_t* image, uint8_t* result, 
            const uint32_t width, const uint32_t height) {
    const uint32_t size{ width * height };
    for (uint32_t pos{width + 1}; pos < size - width - 1; ++pos) {
        int32_t sum = image[pos - width - 1] + (image[pos - width] << 1) + image[pos - width + 1]
            - image[pos + width - 1] - (image[pos + width] << 1) - image[pos + width + 1];
        result[pos] = sum < 0 ? 0 : (sum > 255 ? 255 : sum);
    }
}

void get_max_edge(const uint8_t* edgex_image, const uint8_t* edgey_image,
                  std::vector<uint32_t>& left, std::vector<uint32_t>& right, 
                  std::vector<uint32_t>& front,
		  const uint32_t width, const uint32_t height) {
    for (int row{}; row < height; ++row) {
        const uint32_t row_start{ row * width };
        const uint32_t row_middle{ width >> 1 };

	uint8_t strongest_strength{ edgex_image[row_start + row_middle] };
	uint32_t strongest_edge_pixel{ row_middle };
        double min_stronger_edge_strength{ 
            strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD };
	for (uint32_t column{ row_middle }; column >= 1; --column) {
	    if (edgex_image[row_start + column] >= min_stronger_edge_strength) {
		strongest_strength = edgex_image[row_start + column];
		strongest_edge_pixel = column;
                min_stronger_edge_strength =
                    strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD;
	    }
	}
	left.push_back(strongest_edge_pixel);

	strongest_strength = edgex_image[row_start + row_middle];
	strongest_edge_pixel = row_middle;
        min_stronger_edge_strength = 
            strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD;
	for (uint32_t column{ row_middle }; column < width; ++column) {
	    if (edgex_image[row_start + column] >= min_stronger_edge_strength) {
		strongest_strength = edgex_image[row_start + column];
		strongest_edge_pixel = column;
                min_stronger_edge_strength =
                    strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD;
	    }
	}
	right.push_back(strongest_edge_pixel);
    }
    
    for (int column{}; column < width; ++column) {
	uint8_t strongest_strength{ edgey_image[width * (height - 2) + column] };
	uint32_t strongest_edge_pixel{ height - 2 };
        double min_stronger_edge_strength{ 
            strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD };
	for (uint32_t row{ height - 2 }; row >= (height / 3); --row) {
	    if (edgey_image[row * width + column] >= min_stronger_edge_strength) {
		strongest_strength = edgey_image[row * width + column];
		strongest_edge_pixel = row;
                min_stronger_edge_strength =
                    strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD;
	    }
	}
	front.push_back(strongest_edge_pixel);
    }
}

void mark_edges(const uint8_t* edgex_image, const uint8_t* edgey_image, uint8_t* marked,
		const std::vector<uint32_t>& left, const std::vector<uint32_t>& right,
		const std::vector<uint32_t>& front,
                const uint32_t width, const uint32_t height)
{
    for (uint32_t row{}; row < height; ++row) {
        const uint32_t row_start{ row * width };

	uint32_t strongest_pixel{ left[row] };
	if (edgex_image[row_start + strongest_pixel] > EDGE_STRENGTH_THRESHOLD) {
            const uint32_t marked_pixel_index{ (row_start + strongest_pixel) * 3 };
	    marked[marked_pixel_index] = 255;
	    marked[marked_pixel_index + 1] = 0;
	    marked[marked_pixel_index + 2] = 0;
	}

    	strongest_pixel = right[row];
	if (edgex_image[row_start + strongest_pixel] > EDGE_STRENGTH_THRESHOLD) {
            const uint32_t marked_pixel_index{ (row_start + strongest_pixel) * 3 };
	    marked[marked_pixel_index] = 0;
	    marked[marked_pixel_index + 1] = 255;
	    marked[marked_pixel_index + 2] = 0;
	}
    }
    for (uint32_t column{}; column < width; ++column) {
	uint32_t strongest_pixel{ front[column] };
	if (edgey_image[width * strongest_pixel + column] > EDGE_STRENGTH_THRESHOLD) {
            const uint32_t marked_pixel_index{ (width * strongest_pixel + column) * 3 };
	    marked[marked_pixel_index] = 0;
	    marked[marked_pixel_index + 1] = 0;
	    marked[marked_pixel_index + 2] = 255;
	}
    }
}
