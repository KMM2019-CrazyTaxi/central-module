#include "image_recognition.hpp"

#include <thread>
#include <chrono>
#include <cstring>
#include <vector>
#include <fstream>

#include "logging.hpp"
#include "double_buffer.hpp"
#include "camera_thread.hpp"
#include "image_util.hpp"

using hr_clock = std::chrono::high_resolution_clock;
using time_point = hr_clock::time_point;

const uint32_t WIDTH{ IMAGE_WIDTH };
const uint32_t HEIGHT{ IMAGE_HEIGHT };
const uint32_t COLORS{ 3 };
const int SIZE_RGB{ WIDTH * HEIGHT * COLORS };
const int SIZE_GRAY{ WIDTH * HEIGHT };
const bool WRITE_IMAGE_TO_FILE{ true };

int to_ms(const time_point& time1, const time_point& time2) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count();
}

void image_recognition_main(const std::atomic_bool& running, double_buffer& image_buffer) {

    // Swap the buffers to notify camera thread to grab image
    image_buffer.swap_buffers();

    // Buffers to save partially processed image.
    uint8_t* gray = new uint8_t[SIZE_GRAY];
    uint8_t* blur = new uint8_t[SIZE_GRAY];
    uint8_t* edge = new uint8_t[SIZE_GRAY];
    uint8_t* marked = new uint8_t[SIZE_RGB];

    // Resulting edge distances.
    std::vector<int> left{}, right{};

    int n_processed_images{};

    // Main loop
    while (running) 
    {

        // Log to stdout and log file
        queue_message("IR Tick");

        // Sleep for 1 second
        //std::this_thread::sleep_for(std::chrono::seconds(1));

	// Start time for benchmarking
	time_point start_time{ hr_clock::now() };

	// Read input image.
	const uint8_t* image{ image_buffer.get_read_buffer() };
	std::memcpy(marked, image, SIZE_RGB);
	
	// Request new image while processing this one.
	image_buffer.swap_buffers();
	
	// Process image.
	rgb2gray(marked, gray, WIDTH, HEIGHT);
	time_point gray_time{ hr_clock::now() };
	sobel(gray, edge, WIDTH, HEIGHT);
	time_point sobel_time{ hr_clock::now() };
	get_max_edge(edge, left, right, WIDTH, HEIGHT);
	left.clear();
	right.clear();
	time_point edge_time{ hr_clock::now() };
	mark_edges(edge, marked, left, right, WIDTH, HEIGHT);
	time_point mark_time{ hr_clock::now() };
	time_point stop_time{ mark_time };

	// Log time taken.
	queue_message("Image processed in " + std::to_string(to_ms(start_time, stop_time)) + " ms.");
	queue_message("RGB2GRAY took " + std::to_string(to_ms(start_time, gray_time)) + " ms.");
	queue_message("Sobel took " + std::to_string(to_ms(gray_time, sobel_time)) + " ms.");
	queue_message("Final edge detection took " + std::to_string(to_ms(sobel_time, edge_time)) + " ms.");
	queue_message("Marking test image took " + std::to_string(to_ms(edge_time, mark_time)) + " ms.");

	if (WRITE_IMAGE_TO_FILE) {
            std::ofstream output{ std::to_string(n_processed_images++) + "_processed.ppm", std::ios::binary };
            write_image(marked, output, WIDTH, HEIGHT, 3);
            output.close();
	}
    }

}
