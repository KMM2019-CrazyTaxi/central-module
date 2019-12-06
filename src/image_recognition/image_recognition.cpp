#include "image_recognition.hpp"

#include <thread>
#include <chrono>
#include <cstring>
#include <vector>
#include <fstream>
#include <deque>

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

    // Resulting edge distances. Initialize at middle of each image half.
    std::vector<uint32_t> left_edges(IMAGE_HEIGHT, IMAGE_WIDTH / 4);
    std::vector<uint32_t> right_edges(IMAGE_HEIGHT, (3 * IMAGE_WIDTH) / 4);
    std::vector<uint32_t> front_edges(IMAGE_WIDTH);

    // Previous distance value for rolling average.
    std::deque<double> front_distances{0, 0, 0, 0, 0};

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
	// Request new image.
	image_buffer.swap_buffers();

	// Start time for benchmarking
	start_time = hr_clock::now();

	// Read input image.
	const uint8_t* image{ image_buffer.get_read_buffer() };
	std::memcpy(marked_image, image, IMAGE_SIZE_RGB);
	
	// Process image.
	rgb2gray(marked_image, gray_image, IMAGE_WIDTH, IMAGE_HEIGHT);
	rgb2gray_time = hr_clock::now();

	sobelx(gray_image, edgex_image, IMAGE_WIDTH, IMAGE_HEIGHT);
	sobelx_time = hr_clock::now();
	sobely(gray_image, edgey_image, IMAGE_WIDTH, IMAGE_HEIGHT);
	sobely_time = hr_clock::now();

	get_max_side_edge(edgex_image, left_edges,
                          IMAGE_WIDTH, IMAGE_HEIGHT);
        get_max_side_edge(edgex_image, right_edges,
                          IMAGE_WIDTH, IMAGE_HEIGHT);
        get_max_front_edge(edgey_image, front_edges,
                           IMAGE_WIDTH, IMAGE_HEIGHT);

        const uint32_t distance_end_1{ IMAGE_HEIGHT - BOUND_DISTANCE_1_PIXEL };
        const uint32_t distance_start_1{ distance_end_1 - EDGE_AVG_PIXELS };
        const uint32_t distance_end_2{ IMAGE_HEIGHT - BOUND_DISTANCE_2_PIXEL };
        const uint32_t distance_start_2{ distance_end_2 - EDGE_AVG_PIXELS };
        const uint32_t middle_end{ (IMAGE_WIDTH + EDGE_AVG_PIXELS) >> 1 };
        const uint32_t middle_start{ middle_end - EDGE_AVG_PIXELS };

        double left_pixel_distance_1 = get_distance_to_side(edgex_image, left_edges,
                                                            distance_start_1, distance_end_1,
                                                            IMAGE_WIDTH, IMAGE_HEIGHT, 0);
        double right_pixel_distance_1 = get_distance_to_side(edgex_image, right_edges,
                                                             distance_start_1, distance_end_1,
                                                             IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_WIDTH - 1);
        double left_pixel_distance_2 = get_distance_to_side(edgex_image, left_edges,
                                                            distance_start_2, distance_end_2,
                                                            IMAGE_WIDTH, IMAGE_HEIGHT, 0);
        double right_pixel_distance_2 = get_distance_to_side(edgex_image, right_edges,
                                                             distance_start_2, distance_end_2,
                                                             IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_WIDTH - 1);
        double front_pixel_distance = get_distance_to_stop(edgey_image, front_edges,
                                                           middle_start, middle_end,
                                                           IMAGE_WIDTH, IMAGE_HEIGHT);

        double left_real_distance_1 =
            (IMAGE_WIDTH / 2 - left_pixel_distance_1) * CM_PER_PIXEL_AT_BOUND_DISTANCE_1;
        double right_real_distance_1 =
            (right_pixel_distance_1 - IMAGE_WIDTH / 2) * CM_PER_PIXEL_AT_BOUND_DISTANCE_1;
        double left_real_distance_2 =
            (IMAGE_WIDTH / 2 - left_pixel_distance_2) * CM_PER_PIXEL_AT_BOUND_DISTANCE_2;
        double right_real_distance_2 =
            (right_pixel_distance_2 - IMAGE_WIDTH / 2) * CM_PER_PIXEL_AT_BOUND_DISTANCE_2;
        double adjusted_front_pixel_distance = IMAGE_HEIGHT - front_pixel_distance;

	// Front distance if calculated as the average over five pictures to limit noise.
	front_distances.pop_front();
	front_distances.push_back(adjusted_front_pixel_distance);
	double average_front_pixel_distance{};
	for (const double& d : front_distances)
	{
	    average_front_pixel_distance += d;
	}
	average_front_pixel_distance /= 5;
	edge_time = hr_clock::now();

	telemetrics_data* data{ static_cast<telemetrics_data*>(registry.acquire_data(TELEMETRICS_DATA_ID)) };
	data->dist_left = left_real_distance_1;
	data->dist_right = right_real_distance_1;
	data->dist_stop_line = average_front_pixel_distance;
	registry.release_data(TELEMETRICS_DATA_ID);

        if (OUTPUT_MARKED_IMAGE_TO_FILE) {
            mark_all_edges(marked_image, 
                       left_edges, right_edges, front_edges,
                       IMAGE_WIDTH, IMAGE_HEIGHT);
            mark_selected_edges(marked_image, left_pixel_distance_1,
                                right_pixel_distance_1, front_pixel_distance,
                                distance_start_1, distance_end_1, middle_start, middle_end,
                                IMAGE_WIDTH, IMAGE_HEIGHT);
            mark_selected_edges(marked_image, left_pixel_distance_2,
                                right_pixel_distance_2, front_pixel_distance,
                                distance_start_2, distance_end_2, middle_start, middle_end,
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
                queue_message("  Left edge distance 1: " + std::to_string(left_real_distance_1));
                queue_message("  Right edge distance 1: " + std::to_string(right_real_distance_1));
                queue_message("  Left edge distance 2: " + std::to_string(left_real_distance_2));
                queue_message("  Right edge distance 2: " + std::to_string(right_real_distance_2));
                queue_message("  Front edge distance: " + std::to_string(adjusted_front_pixel_distance));
		queue_message("  Marking test image took "
			      + std::to_string(to_ms(edge_time, mark_time)) + " ms.");
		std::string file_name{ std::to_string(n_processed_images / CAMERA_FPS) 
                                      + "_processed.ppm" };
		queue_message("  Saving marked image to " + file_name);
		std::ofstream output{ file_name, std::ios::binary };
		write_image(marked_image, output, IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_TYPE::RGB);
		output.close();
	    }
	}
    }
}
