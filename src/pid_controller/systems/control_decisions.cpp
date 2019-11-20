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

  pid_line_in line_in;

  switch(dec.sys) {

  case turning:
    {
      // Regulate turn
      pid_turning_in turn_in =
	{
	 .metrics = dec.metrics,
	 .params = dec.params.turning,
	 .turn = dec.turn
	};
      pid_turning_out turn_out = pid_turning(turn_in);

      // Set input for line system
      pid_line_in tmp =
	{
	 .angle = turn_out.angle,
	 .speed = turn_out.speed
	};
      line_in = tmp;
      
    }break;

  case parking:
    {

      // Regulate parking
      pid_parking_in park_in =
	{
	 .metrics = dec.metrics,
	 .params = dec.params.parking
	};
      pid_parking_out park_out = pid_parking(park_in);

      // Set input for line system
      pid_line_in tmp =
	{
	 .angle = park_out.angle,
	 .speed = park_out.speed
	};
      line_in = tmp;
      
    }break;

  case stopping:
    {

      // Regulate stopping
      pid_stopping_in stop_in =
	{
	 .metrics = dec.metrics,
	 .params = dec.params.stopping
	};
      pid_stopping_out stop_out = pid_stopping(stop_in);

      // set input for line system
      pid_line_in tmp =
	{
	 .angle = stop_out.angle,
	 .speed = stop_out.speed
	};
      line_in = tmp;
      
    }break;

  case line:
    {

      // Set input for line system
      pid_line_in tmp =
	{
	 .angle = dec.metrics.curr_angle,
	 .speed = dec.metrics.curr_speed,
	 .params = dec.params.line
	};
      line_in = tmp;
      
    }break;
    
  default:
    {
      queue_message("Regulator sent a request for a non-existing system: " + std::to_string(dec.sys));
    }break;
    
  }

  pid_line_out line_out = pid_line(line_in);
  regulator_out_data out =
    {
     .angle = line_out.angle,
     .speed = line_out.speed
    };

  return out;
}
