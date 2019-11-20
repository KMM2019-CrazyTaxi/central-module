#include "control_decisions.hpp"
#include "logging.hpp"

pid_decision_data decide(pid_decision_in);
regulator_out_data regulate(pid_decision_data);


regulator_out_data pid_decision(pid_decision_in in) {
  pid_decision_data data = decide(in);
  regulator_out_data out = regulate(data);
  return out;
}

pid_decision_data decide(pid_decision_in in) {
  // TODO: Implement decision making
  pid_decision_data data =
    {
     .sys = line,
     .turn = left
    };
  return data;
}

regulator_out_data regulate(pid_decision_data dec) {

  pid_system_out line_in;

  switch(dec.sys) {

  case turning:
    {
      // Regulate turn
      pid_system_out turning = pid_turning(dec);

      // Set input for line system
      line_in = turning;
      
    }break;

  case parking:
    {

      // Regulate parking
      pid_system_out parking = pid_parking(dec);

      // Set input for line system
      line_in = parking;
      
    }break;

  case stopping:
    {

      // Regulate stopping
      pid_system_out stopping = pid_stopping(dec);

      // set input for line system
      line_in = stopping;
      
    }break;

  case line:
    {

      // Set input for line system
      line_in = dec.out;
      
    }break;
    
  default:
    {
      queue_message("Regulator sent a request for a non-existing system: " + std::to_string(dec.sys));
    }break;
    
  }

  pid_system_out line_out = pid_line(line_in);
  regulator_out_data out =
    {
     .angle = line_out.angle,
     .speed = line_out.speed
    };

  return out;
}
