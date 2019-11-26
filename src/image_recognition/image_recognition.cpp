#include "image_recognition.hpp"

#include <thread>
#include <chrono>
#include <cstring>
#include <vector>
#include <fstream>

#include "data_registry.hpp"
#include "registry_entries.hpp"
#include "logging.hpp"
#include "double_buffer.hpp"
#include "camera_thread.hpp"
#include "image_util.hpp"
#include "image_recognition_constants.hpp"

using hr_clock = std::chrono::high_resolution_clock;
using time_point = hr_clock::time_point;

// Returns the time difference in ms between to time points.
int to_ms(const time_point& time1, const time_point& time2) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count();
}

void image_recognition_main(const std::atomic_bool& running, double_buffer& image_buffer) {

    // Swap the buffers to notify camera thread to grab image
    image_buffer.swap_buffers();

    data_registry& registry{ data_registry::get_instance() };

    // Buffers to save partially processed image.
    uint8_t* gray_image = new uint8_t[IMAGE_SIZE_GRAY];
    uint8_t* edgex_image = new uint8_t[IMAGE_SIZE_GRAY];
    uint8_t* edgey_image = new uint8_t[IMAGE_SIZE_GRAY];
    uint8_t* marked_image = new uint8_t[IMAGE_SIZE_RGB];

    // Resulting edge distances.
    std::vector<uint32_t> left_edges{};
    std::vector<uint32_t> right_edges{};
    std::vector<uint32_t> front_edges{};

    uint32_t n_processed_images{};

    time_point start_time{};
    time_point rgb2gray_time{};
    time_point sobelx_time{};
    time_point sobely_time{};
    time_point edge_time{};
    time_point mark_time{};
    time_point stop_time{};

    // Sleep for 1 second to wait for camera to init.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Main loop
    while (running) 
    {

	// Start time for benchmarking
	start_time = hr_clock::now();

	// Read input image.
	const uint8_t* image{ image_buffer.get_read_buffer() };
	std::memcpy(marked_image, image, IMAGE_SIZE_RGB);
	
	// Request new image while processing this one.
	image_buffer.swap_buffers();

	// Process image.
	rgb2gray(marked_image, gray_image, IMAGE_WIDTH, IMAGE_HEIGHT);
	rgb2gray_time = hr_clock::now();

	sobelx(gray_image, edgex_image, IMAGE_WIDTH, IMAGE_HEIGHT);
	sobelx_time = hr_clock::now();
	sobely(gray_image, edgey_image, IMAGE_WIDTH, IMAGE_HEIGHT);
	sobely_time = hr_clock::now();

	left_edges.clear();
	right_edges.clear();
	front_edges.clear();
	get_max_edge(edgex_image, edgey_image,
		     left_edges, right_edges, front_edges,
                     IMAGE_WIDTH, IMAGE_HEIGHT);

        const uint32_t distance_end{ IMAGE_HEIGHT - BOUND_DISTANCE_PIXEL };
        const uint32_t distance_start{ distance_end - EDGE_AVG_PIXELS };
        const uint32_t middle_end{ (IMAGE_WIDTH + EDGE_AVG_PIXELS) >> 1 };
        const uint32_t middle_start{ middle_end - EDGE_AVG_PIXELS };
        double left_pixel_distance = get_distance_to_side(edgex_image, left_edges,
                                                          distance_start, distance_end,
                                                          IMAGE_WIDTH, IMAGE_HEIGHT);
        double left_real_distance =
            (IMAGE_WIDTH / 2 - left_pixel_distance) * CM_PER_PIXEL_AT_BOUND_DISTANCE;
        double right_pixel_distance = get_distance_to_side(edgex_image, right_edges,
                                                           distance_start, distance_end,
                                                           IMAGE_WIDTH, IMAGE_HEIGHT);
        double right_real_distance =
            (right_pixel_distance - IMAGE_WIDTH / 2) * CM_PER_PIXEL_AT_BOUND_DISTANCE;
        double front_pixel_distance = get_distance_to_stop(edgey_image, front_edges,
                                                           middle_start, middle_end,
                                                           IMAGE_WIDTH, IMAGE_HEIGHT);
	double adjusted_front_pixel_distance = IMAGE_HEIGHT - front_pixel_distance;
	edge_time = hr_clock::now();

	telemetrics_data* data{ static_cast<telemetrics_data*>(registry.acquire_data(TELEMETRICS_DATA_ID)) };
	data->dist_left = left_real_distance;
	data->dist_right = right_real_distance;
	data->dist_stop_line = front_pixel_distance;
	registry.release_data(TELEMETRICS_DATA_ID);

        if (OUTPUT_MARKED_IMAGE_TO_FILE) {
/*
            mark_edges(edgex_image, edgey_image, marked_image, 
                       left_edges, right_edges, front_edges,
                       IMAGE_WIDTH, IMAGE_HEIGHT);
*/
            mark_selected_edges(marked_image,
                                left_pixel_distance, right_pixel_distance, front_pixel_distance,
                                distance_start, distance_end, middle_start, middle_end,
                                IMAGE_WIDTH, IMAGE_HEIGHT);
            mark_time = hr_clock::now();
        }
	stop_time = hr_clock::now();

        // Log information once per second.
	if (n_processed_images++ % CAMERA_FPS == 0) {
	    queue_message("Image processed in "
			  + std::to_string(to_ms(start_time, stop_time)) + " ms.");
	    queue_message("  RGB2GRAY took "
			  + std::to_string(to_ms(start_time, rgb2gray_time)) + " ms.");
	    queue_message("  Sobelx took "
			  + std::to_string(to_ms(rgb2gray_time, sobelx_time)) + " ms.");
	    queue_message("  Sobely took "
			  + std::to_string(to_ms(sobelx_time, sobely_time)) + " ms.");
	    queue_message("  Final edge detection took " 
			  + std::to_string(to_ms(sobely_time, edge_time)) + " ms.");

	    if (OUTPUT_MARKED_IMAGE_TO_FILE) {
                queue_message("  Left edge distance: " + std::to_string(left_real_distance));
                queue_message("  Right edge distance: " + std::to_string(right_real_distance));
                queue_message("  Front edge distance: " + std::to_string(IMAGE_HEIGHT - front_pixel_distance));
		queue_message("  Marking test image took "
			      + std::to_string(to_ms(edge_time, mark_time)) + " ms.");
		std::string file_name{ std::to_string(n_processed_images / CAMERA_FPS) 
                                       + "_processed.ppm" };
		std::ofstream output{ file_name, std::ios::binary };
		write_image(marked_image, output, IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_TYPE::RGB);
		output.close();
	    }
	}
    }
}
