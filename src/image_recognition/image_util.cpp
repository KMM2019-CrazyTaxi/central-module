#ifdef OPENCV
#include <opencv2/opencv.hpp>
#endif

#include <fstream>

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

void write_image(const uint8_t* image, const std::string& file_name,
		 const uint32_t width, const uint32_t height, const IMAGE_TYPE type) {
    uint32_t size{};
    std::string format{};
#ifdef OPENCV
    cv::Mat cv_image;
#endif

    switch (type) {
    case GRAY:
#ifdef OPENCV
        cv_image = cv::Mat(height, width, CV_8UC1, (void*) image);
        cv_image.reshape(1, height);
#endif
        size = width * height;
	format = "P5\n";
        break;
    case RGB:
#ifdef OPENCV
        cv_image = cv::Mat(height, width, CV_8UC3, (void*) image);
        cv_image.reshape(3, height);
#endif
        size = width * height * 3;
	format = "P6\n";
        break;
    }

#ifdef OPENCV
    cv::imwrite(file_name, cv_image);
#else
    std::ofstream output{ file_name, std::ios::binary };
    output << format << width << " " << height << " 255\n";
    output.write((char*) image, size);
    output.close();
#endif
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
        *(gray++) = sum >= 255 ? 255 : sum;
    }
}

void sobelx(const uint8_t* image, uint8_t* result, 
            const uint32_t width, const uint32_t height) {
    const uint32_t size{ width * height };
    for (uint32_t pos{width + 1}; pos < size - width - 1; ++pos) {
        int32_t sum = image[pos - width - 1] - image[pos - width + 1]
            + (image[pos - 1] << 1) - (image[pos + 1] << 1)
            + image[pos + width - 1] - image[pos + width + 1];
        sum >>= 1;
        result[pos] = sum < 0 ? 0 : (sum > 255 ? 255 : sum);
    }
}

void sobely(const uint8_t* image, uint8_t* result, 
            const uint32_t width, const uint32_t height) {
    const uint32_t size{ width * height };
    for (uint32_t pos{width + 1}; pos < size - width - 1; ++pos) {
        int32_t sum = image[pos - width - 1] + (image[pos - width] << 1) + image[pos - width + 1]
            - image[pos + width - 1] - (image[pos + width] << 1) - image[pos + width + 1];
        sum /= 2;
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
	for (uint32_t column{ 1 }; column < row_middle; ++column) {
	    if (edgex_image[row_start + column] > min_stronger_edge_strength) {
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
	for (uint32_t column{ width - 2 }; column > row_middle; --column) {
	    if (edgex_image[row_start + column] > min_stronger_edge_strength) {
		strongest_strength = edgex_image[row_start + column];
		strongest_edge_pixel = column;
                min_stronger_edge_strength =
                    strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD;
	    }
	}
	right.push_back(strongest_edge_pixel);
    }
    
    const uint32_t range_start{ height - STOP_LINE_START_DISTANCE };
    const uint32_t range_end{ height - STOP_LINE_END_DISTANCE };
    for (int column{}; column < width; ++column) {
	uint32_t strongest_edge_pixel{ range_start };
	uint8_t strongest_strength{ edgey_image[width * strongest_edge_pixel + column] };
        double min_stronger_edge_strength{ 
            strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD };
	for (uint32_t row{ range_start }; row >= range_end; --row) {
	    if (edgey_image[row * width + column] > min_stronger_edge_strength) {
		strongest_strength = edgey_image[row * width + column];
		strongest_edge_pixel = row;
                min_stronger_edge_strength =
                    strongest_strength * RELATIVE_EDGE_STRENGTH_THRESHOLD;
	    }
	}
	front.push_back(strongest_edge_pixel);
    }
}

double get_distance_to_side(const uint8_t* edge_image, std::vector<uint32_t>& best,
                            const uint32_t start_row, const uint32_t end_row,
                            const uint32_t width, const uint32_t height,
			    const double default_edge)
{
    double weighted_edge_location_sum{};
    double edge_strength_sum{};
    for (uint32_t row{start_row}; row <= end_row; ++row)
    {
        const double edge_strength = edge_image[row * width + best[row]];
        weighted_edge_location_sum += edge_strength * best[row];
        edge_strength_sum += edge_strength;
    }

    // Ignore low strength edges.
    if (edge_strength_sum < EDGE_STRENGTH_THRESHOLD * (end_row - start_row + 1))
    {
	return default_edge;
    }
    else
    {
	return weighted_edge_location_sum / edge_strength_sum;
    }
}

double get_distance_to_stop(const uint8_t* edge_image, std::vector<uint32_t>& best,
                            const uint32_t start_column, const uint32_t end_column,
                            const uint32_t width, const uint32_t height)
{
    double weighted_edge_location_sum{};
    double edge_location_sum{};
    for (uint32_t column{start_column}; column <= end_column; ++column)
    {
        const double edge_strength = edge_image[best[column] * width + column];
        weighted_edge_location_sum += edge_strength * best[column];
        edge_location_sum += edge_strength;
    }
    if (edge_location_sum < EDGE_STRENGTH_THRESHOLD * (end_column - start_column + 1))
    {
        return 0;
    }
    else
    {
        return weighted_edge_location_sum / edge_location_sum;
    }
}

void mark_selected_edges(uint8_t* marked,
                         const double left, const double right, const double front,
                         const uint32_t start_row, const uint32_t end_row,
                         const uint32_t start_column, const uint32_t end_column,
                         const uint32_t width, const uint32_t height)
{
    for (uint32_t row{start_row}; row <= end_row; ++row) {
        uint32_t marked_pixel_index{ 3* row * width + 3 * static_cast<uint32_t>(left) };
        marked[marked_pixel_index] = 255;
        marked[marked_pixel_index + 1] = 0;
        marked[marked_pixel_index + 2] = 0;

        marked_pixel_index = 3 * row * width + 3 * static_cast<uint32_t>(right);
        marked[marked_pixel_index] = 0;
        marked[marked_pixel_index + 1] = 255;
        marked[marked_pixel_index + 2] = 0;
    }

    for (uint32_t column{ start_column }; column <= end_column; ++column) {
        const uint32_t marked_pixel_index{ 3 * width * static_cast<uint32_t>(front) + column * 3 };
        marked[marked_pixel_index] = 0;
        marked[marked_pixel_index + 1] = 0;
        marked[marked_pixel_index + 2] = 255;        
    }
}

void mark_all_edges(const uint8_t* edgex_image, const uint8_t* edgey_image, uint8_t* marked,
                    const std::vector<uint32_t>& left, const std::vector<uint32_t>& right,
                    const std::vector<uint32_t>& front,
                    const uint32_t width, const uint32_t height)
{
    for (uint32_t row{}; row < height; ++row) {
        const uint32_t row_start{ row * width };

	uint32_t strongest_pixel{ left[row] };
        uint32_t marked_pixel_index{ (row_start + strongest_pixel) * 3 };
        marked[marked_pixel_index] = 255;
        marked[marked_pixel_index + 1] = 0;
        marked[marked_pixel_index + 2] = 0;

    	strongest_pixel = right[row];
        marked_pixel_index = (row_start + strongest_pixel) * 3;
        marked[marked_pixel_index] = 0;
        marked[marked_pixel_index + 1] = 255;
        marked[marked_pixel_index + 2] = 0;
    }
    for (uint32_t column{}; column < width; ++column) {
	uint32_t strongest_pixel{ front[column] };
        const uint32_t marked_pixel_index{ (width * strongest_pixel + column) * 3 };
        marked[marked_pixel_index] = 0;
        marked[marked_pixel_index + 1] = 0;
        marked[marked_pixel_index + 2] = 255;
    }
}
