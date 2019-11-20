#include "pid_controller.hpp"
#include "control_decisions.hpp"

#include "registry_entries.hpp"
#include "data_registry.hpp"

#include "logging.hpp"

telemetrics_data get_metrics();
regulator_param_data get_params();
void set_output(regulator_out_data);


void pid_ctrl_thread_main(const std::atomic_bool& running){

  while (running) {

    // Get all data for the regulator
    telemetrics_data metrics = get_metrics();
    regulator_param_data params = get_params();

    pid_decision_in dec_in =
      {
       .metrics = metrics,
       .params = params
      };

    // Regulate
    regulator_out_data reg_out;
    reg_out = pid_decision(dec_in);

    // Send output
    set_output(reg_out);
      
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

void set_output(regulator_out_data output){
    regulator_out_data *registry_entry =
      (regulator_out_data*) data_registry::get_instance().
      acquire_data(REGULATOR_OUT_DATA_ID);
    *registry_entry = output;
    data_registry::get_instance().release_data(REGULATOR_OUT_DATA_ID);
    
}

