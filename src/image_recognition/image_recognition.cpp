#include "image_recognition.hpp"

#include <thread>
#include <chrono>
#include <cstring>
#include <vector>
#include <fstream>

#ifdef QPU

#include "QPULib.h"

#endif

#include "logging.hpp"
#include "double_buffer.hpp"
#include "camera_thread.hpp"
#include "image_util.hpp"

using hr_clock = std::chrono::high_resolution_clock;
using time_point = hr_clock::time_point;

const int32_t WIDTH{ IMAGE_WIDTH };
const int32_t HEIGHT{ IMAGE_HEIGHT };
const int32_t COLORS{ 3 };
const int SIZE_RGB{ WIDTH * HEIGHT * COLORS };
const int SIZE_GRAY{ WIDTH * HEIGHT };
const bool WRITE_IMAGE_TO_FILE{ true };
const int NUM_QPU{ 4 };

int to_ms(const time_point& time1, const time_point& time2) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count();
}

void image_recognition_main(const std::atomic_bool& running, double_buffer& image_buffer) {

    // Swap the buffers to notify camera thread to grab image
    image_buffer.swap_buffers();

    // Buffers to save partially processed image.
#ifdef QPU
    SharedArray<int> qpu_image(SIZE_RGB);
    SharedArray<int> qpu_gray(SIZE_GRAY);
    SharedArray<int> qpu_edge(SIZE_GRAY);
#endif
    uint8_t* gray = new uint8_t[SIZE_GRAY];
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
        std::this_thread::sleep_for(std::chrono::seconds(1));

	// Start time for benchmarking
	time_point start_time{ hr_clock::now() };

	// Read input image.
	const uint8_t* image{ image_buffer.get_read_buffer() };
	std::memcpy(marked, image, SIZE_RGB);
	
	// Request new image while processing this one.
	image_buffer.swap_buffers();

	// Process image.
#ifdef QPU
        for (int i{}; i < SIZE_RGB; ++i) {
            qpu_image[i] = marked[i];
        }

        auto rgb2gray = compile(rgb2gray_qpu);
        rgb2gray.setNumQPUs(NUM_QPU);
        rgb2gray(&qpu_image, &qpu_gray, WIDTH, HEIGHT);
        time_point gray_time{ hr_clock::now() };

        for (int i{}; i < SIZE_GRAY; ++i) {
            gray[i] = qpu_gray[i];
        }

        auto sobel = compile(sobel_qpu);
        sobel.setNumQPUs(NUM_QPU);
        sobel(&qpu_gray, &qpu_edge, WIDTH, HEIGHT);
	time_point sobel_time{ hr_clock::now() };

        for (int i{}; i < SIZE_GRAY; ++i) {
            edge[i] = static_cast<uint8_t>(qpu_edge[i]);
        }
#else
	rgb2gray(marked, gray, WIDTH, HEIGHT);
	time_point gray_time{ hr_clock::now() };
	sobel(gray, edge, WIDTH, HEIGHT);
	time_point sobel_time{ hr_clock::now() };
#endif
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
            write_image(gray, output, WIDTH, HEIGHT, 1);
            output.close();
	}
    }

}
