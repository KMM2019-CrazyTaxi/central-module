#include "pid_controller.hpp"
#include "control_decisions.hpp"

#include "update_controller.hpp"
#include "registry_entries.hpp"
#include "data_registry.hpp"

#include "logging.hpp"

telemetrics_data get_metrics();
regulator_param_data get_params();
regulator_sample_data get_samples();
void set_output(regulator_out_data);
void set_samples(regulator_sample_data);


void pid_ctrl_thread_main(const std::atomic_bool& running){

  update_controller upd_controller{};

  auto previous_time = std::chrono::steady_clock::now();

  while (running) {

    auto current_time = std::chrono::steady_clock::now();

    upd_controller.start();

    // Get all data for the regulator
    telemetrics_data metrics = get_metrics();
    regulator_param_data params = get_params();
    
    regulator_sample_data samples = get_samples();
    
    double dt = std::chrono::duration_cast<std::chrono::seconds>(current_time - previous_time).count();

    pid_decision_in dec_in =
      {
       .metrics = metrics,
       .params = params,
       .dt = dt,
       .samples = samples
      };

    // Regulate
    pid_decision_return regulate = pid_decision(dec_in);

    samples = regulate.samples;
    
    regulator_out_data reg_out =
      {
       .angle = regulate.angle,
       .speed = regulate.speed
      };

    // Send output
    set_output(reg_out);
    set_samples(samples);
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

