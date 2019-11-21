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
#include "image_recognition_constants.hpp"

using hr_clock = std::chrono::high_resolution_clock;
using time_point = hr_clock::time_point;

int to_ms(const time_point& time1, const time_point& time2) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count();
}

void image_recognition_main(const std::atomic_bool& running, double_buffer& image_buffer) {

    // Swap the buffers to notify camera thread to grab image
    image_buffer.swap_buffers();

    // Buffers to save partially processed image.
#ifdef QPU
    SharedArray<int> qpu_image(IMAGE_SIZE_RGB);
    SharedArray<int> qpu_gray(IMAGE_SIZE_GRAY);
    SharedArray<int> qpu_edge(IMAGE_SIZE_GRAY);
#endif

    uint8_t* gray_image = new uint8_t[IMAGE_SIZE_GRAY];
    uint8_t* edge_image = new uint8_t[IMAGE_SIZE_GRAY];
    uint8_t* marked_image = new uint8_t[IMAGE_SIZE_RGB];

    // Resulting edge distances.
    std::vector<uint32_t> left_edges{};
    std::vector<uint32_t> right_edges{};

    uint32_t n_processed_images{};

    time_point start_time{};
    time_point rgb2gray_time{};
    time_point sobel_time{};
    time_point edge_time{};
    time_point mark_time{};
    time_point stop_time{};

    // Main loop
    while (running) 
    {

        // Log to stdout and log file
        queue_message("IR Tick");

        // Sleep for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));

	// Start time for benchmarking
	start_time = hr_clock::now();

	// Read input image.
	const uint8_t* image{ image_buffer.get_read_buffer() };
	std::memcpy(marked_image, image, IMAGE_SIZE_RGB);
	
	// Request new image while processing this one.
	image_buffer.swap_buffers();

	// Process image.
#ifdef QPU
        for (int i{}; i < IMAGE_SIZE_RGB; ++i) {
            qpu_image[i] = marked[i];
        }

        auto rgb2gray = compile(rgb2gray_qpu);
        rgb2gray.setNumQPUs(NUM_QPU);
        rgb2gray(&qpu_image, &qpu_gray, IMAGE_WIDTH, IMAGE_HEIGHT);
        gray_time = hr_clock::now();

        auto sobel = compile(sobel_qpu);
        sobel.setNumQPUs(NUM_QPU);
        sobel(&qpu_gray, &qpu_edge, WIDTH, HEIGHT);
	sobel_time = hr_clock::now();

        for (int i{}; i < IMAGE_SIZE_GRAY; ++i) {
            edge[i] = static_cast<uint8_t>(qpu_edge[i]);
        }
#else
	rgb2gray(marked_image, gray_image, IMAGE_WIDTH, IMAGE_HEIGHT);
	time_point gray_time = hr_clock::now();
	sobelx(gray_image, edge_image, IMAGE_WIDTH, IMAGE_HEIGHT);
	time_point sobel_time = hr_clock::now();
#endif
	left_edges.clear();
	right_edges.clear();
	get_max_edge(edge_image, left_edges, right_edges,
                     IMAGE_WIDTH, IMAGE_HEIGHT);
	edge_time = hr_clock::now();
        if (OUTPUT_MARKED_IMAGE_TO_FILE) {
            mark_edges(edge_image, marked_image, left_edges, right_edges,
                       IMAGE_WIDTH, IMAGE_HEIGHT);
            mark_time = hr_clock::now();
        }
	stop_time = hr_clock::now();

	// Log time taken.
	queue_message("Image processed in "
                      + std::to_string(to_ms(start_time, stop_time)) + " ms.");
	queue_message("  RGB2GRAY took "
                      + std::to_string(to_ms(start_time, gray_time)) + " ms.");
	queue_message("  Sobel took "
                      + std::to_string(to_ms(gray_time, sobel_time)) + " ms.");
	queue_message("  Final edge detection took " 
                      + std::to_string(to_ms(sobel_time, edge_time)) + " ms.");
        if (OUTPUT_MARKED_IMAGE_TO_FILE) {
            queue_message("  Marking test image took "
                          + std::to_string(to_ms(edge_time, mark_time)) + " ms.");
            std::string file_name{ std::to_string(n_processed_images++) + "_processed.ppm" };
            std::ofstream output{ file_name, std::ios::binary };
            write_image(marked_image, output, IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_TYPE::RGB);
            output.close();
	}
    }
}
