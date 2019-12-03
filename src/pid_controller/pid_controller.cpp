#include "pid_controller.hpp"
#include "control_decisions.hpp"

#include "update_controller.hpp"
#include "registry_entries.hpp"
#include "data_registry.hpp"

#include "logging.hpp"

// Forward declarations
telemetrics_data get_metrics();
regulator_param_data get_params();
regulator_sample_data get_samples();
mission_data get_mission_data();
std::vector<path_step> get_path();
void set_output(regulator_out_data);
void set_samples(regulator_sample_data);
void set_mission_data(mission_data);
void set_path(std::vector<path_step>);


void pid_ctrl_thread_main(const std::atomic_bool& running){

  update_controller upd_controller{};

  auto previous_time = std::chrono::steady_clock::now();

  while (running) {

    mission_data mission_data = get_mission_data();
    // Check if we have any current missions to run
    //if (mission_data.missions.size() == 0) continue;

    auto current_time = std::chrono::steady_clock::now();

    upd_controller.start();

    // Get all data for the regulator
    telemetrics_data metrics = get_metrics();
    regulator_param_data params = get_params();
    regulator_sample_data samples = get_samples();
    std::vector<path_step> path = get_path();

    // Set deltatime
    double dt = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - previous_time).count();

    std::pair<int, int> mission;// = mission_data.missions[0];
    mission.first = 1;
    mission.second = 5;
    /*

    // If we are not already at the start position for some reason, go there
    if (mission_data.current_pos != mission.first && path.back().node != mission.second)
        path = find_shortest_path(mission_data.g, mission_data.current_pos,
                                    mission.first);
        // @TODO: Push the new mission to go to the start point to the front
    // Or if we haven't found the path yet, do it.
    else if (path.back().node != mission.second)
        path = find_shortest_path(mission_data.g, mission_data.current_pos,
                                    mission.second);
    set_path(path);
    */

    // Define input to the regulator
    pid_decision_in dec_in =
      {
       .metrics = metrics,
       .params = params,
       .dt = dt,
       .samples = samples,
       .g = mission_data.g,
       .path = path,
       .current_pos = mission_data.current_pos
      };

    // Regulate
    pid_decision_return regulate = pid_decision(dec_in);

    // Stay at the end for some time, then continue
    if (mission_data.current_pos == mission.second && metrics.curr_speed <= 1){
        std::this_thread::sleep_for(std::chrono::seconds(3));
        mission_data.missions.erase(mission_data.missions.begin());
    }

    // Update current position
    mission_data.current_pos = regulate.current_pos;

    samples = regulate.samples;

    /*
    // Bind the angle
    if (regulate.angle > MAX_INPUT_ANGLE) regulate.angle = MAX_INPUT_ANGLE;
    else if (regulate.angle < -MAX_INPUT_ANGLE) regulate.angle = -MAX_INPUT_ANGLE;

    // Bind the speed
    if (regulate.speed > MAX_INPUT_SPEED) regulate.speed = MAX_INPUT_SPEED;
    else if (regulate.angle < -MAX_INPUT_SPEED) regulate.speed = -MAX_INPUT_SPEED;
    */

    queue_message("Reg speed: " + std::to_string(regulate.speed));
    queue_message("Reg angle: " + std::to_string(regulate.angle));

    regulator_out_data reg_out =
      {
       .angle = regulate.angle,
       .speed = regulate.speed
      };

    // Send output
    set_output(reg_out);
    set_samples(samples);
    set_mission_data(mission_data);
    previous_time = current_time;

    upd_controller.wait();
  }

}

// Get metrics from global registry
telemetrics_data get_metrics(){
    telemetrics_data metrics;

    telemetrics_data *registry_entry =
      (telemetrics_data*) data_registry::get_instance().
      acquire_data(TELEMETRICS_DATA_ID);
    metrics = *registry_entry;
    data_registry::get_instance().release_data(TELEMETRICS_DATA_ID);
    return metrics;
}

// Get params from global registry
regulator_param_data get_params(){
    regulator_param_data params;

    regulator_param_data *registry_entry =
      (regulator_param_data*) data_registry::get_instance().
      acquire_data(REGULATOR_PARAM_DATA_ID);
    params = *registry_entry;
    data_registry::get_instance().release_data(REGULATOR_PARAM_DATA_ID);
    return params;
}

// Get params from global registry
regulator_sample_data get_samples(){
    regulator_sample_data samples;

    regulator_sample_data *registry_entry =
      (regulator_sample_data*) data_registry::get_instance().
      acquire_data(REGULATOR_SAMPLE_DATA_ID);
    samples = *registry_entry;
    data_registry::get_instance().release_data(REGULATOR_SAMPLE_DATA_ID);
    return samples;
}

// Get mission data from global registry
mission_data get_mission_data(){
    mission_data data;

    mission_data *registry_entry =
      (mission_data*) data_registry::get_instance().
      acquire_data(MISSION_DATA_ID);
    data = *registry_entry;
    data_registry::get_instance().release_data(MISSION_DATA_ID);
    return data;
}

// Get path from global registry
std::vector<path_step> get_path(){
    std::vector<path_step> p;

    std::vector<path_step> *registry_entry =
      (std::vector<path_step>*) data_registry::get_instance().
      acquire_data(PATH_ID);
    p = *registry_entry;
    data_registry::get_instance().release_data(PATH_ID);
    return p;
}

void set_output(regulator_out_data output){
    regulator_out_data *registry_entry =
      (regulator_out_data*) data_registry::get_instance().
      acquire_data(REGULATOR_OUT_DATA_ID);
    *registry_entry = output;
    data_registry::get_instance().release_data(REGULATOR_OUT_DATA_ID);

}

void set_samples(regulator_sample_data samples){
    regulator_sample_data *registry_entry =
      (regulator_sample_data*) data_registry::get_instance().
      acquire_data(REGULATOR_SAMPLE_DATA_ID);
    *registry_entry = samples;
    data_registry::get_instance().release_data(REGULATOR_SAMPLE_DATA_ID);

}

void set_mission_data(mission_data data){
    mission_data *registry_entry =
      (mission_data*) data_registry::get_instance().
      acquire_data(MISSION_DATA_ID);
    *registry_entry = data;
    data_registry::get_instance().release_data(MISSION_DATA_ID);
}

void set_path(std::vector<path_step> path){
    std::vector<path_step> *registry_entry =
      (std::vector<path_step>*) data_registry::get_instance().
      acquire_data(PATH_ID);
    *registry_entry = path;
    data_registry::get_instance().release_data(PATH_ID);
}
