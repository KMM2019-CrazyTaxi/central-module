#include "pid_controller.hpp"
#include "logging.hpp"
#include "registry_entries.hpp"
#include "data_registry.hpp"

regulator_data get_metrics();
void set_output(regulator_output);

void pid_ctrl_thread_main(const std::atomic_bool& running){

  while (running) {

    regulator_data metrics = get_metrics();

    // Regulate Turn
    
    ctrl_turn_metrics turn_metrics =
      {
       .curr_turn = metrics.curr_turn,
       .input_turn = metrics.input_turn,
       .dist_left = metrics.dist_left,
       .dist_right = metrics.dist_right
      };

    uint8_t regulated_turn = regulate_turn(turn_metrics);

    // Regulate Speed

    ctrl_speed_metrics speed_metrics =
      {
       .curr_speed = metrics.curr_speed,
       .input_speed = metrics.input_speed,
       .turn = regulated_turn
      };

    uint8_t regulated_speed = regulate_speed(speed_metrics);

    // Set output in global registry
    regulator_output output =
      {
       .speed = regulated_speed,
       .turn = regulated_turn
      };

    set_output(output);
  }
  
}

uint8_t regulate_turn(ctrl_turn_metrics metrics) {
  return 0;
}

uint8_t regulate_speed(ctrl_speed_metrics metrics) {
  return 0;
}

regulator_data get_metrics(){
  // Get metrics from global registry
    regulator_data metrics;

    regulator_data *registry_entry =
      (regulator_data*) data_registry::get_instance().
      acquire_data(REGULATOR_DATA_ID);
    metrics = *registry_entry;
    data_registry::get_instance().release_data(REGULATOR_DATA_ID);
    return metrics;
}

void set_output(regulator_output output){
    regulator_output *registry_entry =
      (regulator_output*) data_registry::get_instance().
      acquire_data(REGULATOR_OUTPUT_ID);
    *registry_entry = output;
    data_registry::get_instance().release_data(REGULATOR_OUTPUT_ID);
}
