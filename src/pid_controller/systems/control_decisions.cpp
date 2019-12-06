#include "control_decisions.hpp"
#include "logging.hpp"

pid_decision_data decide(pid_decision_in &);
pid_decision_return regulate(pid_decision_data &);


pid_decision_return pid_decision(pid_decision_in &in) {
    pid_decision_data data = decide(in);
    pid_decision_return out = regulate(data);
    out.current_pos = data.map.current_pos;
    return out;
}

pid_decision_data decide(pid_decision_in &in) {

    // Default settings for line following
    pid_decision_data data =
        {
        .sys = line,
        .map.g = in.map.g,
        .map.path = in.map.path,
        .map.current_pos = in.map.current_pos,
        .out.metrics = in.metrics,
        .out.params = in.params,
        .out.samples = in.samples,
        .out.samples.dist_stop_line = in.metrics.dist_stop_line,
        .out.dt = in.dt,
        .out.angle = 0,
        .out.speed = 10
        };

    // If an obstacle is ahead, we stop
    // TODO: Update values, don't know what's reasonable
    if (in.sensor_data.dist < 400) {
        data.sys = stopping;
        data.out.speed = 0;
        data.dist = (double)in.sensor_data.dist;
    }

    // If the next stop line is far away, return line follower
    // if (in.metrics.dist_stop_line > INC_POS_UPPER_LIMIT) return data;

    int current_pos = in.map.current_pos;
    queue_message("Current pos: " + std::to_string(current_pos));

    // If distance to stop line increased, we assume we passed one.
    // This also means that the initial sample value should be big.
    double curr_line_height = in.metrics.dist_stop_line;
    double prev_line_height = in.samples.dist_stop_line;

    if (curr_line_height > prev_line_height + INC_POS_ERROR_DELTA &&
            prev_line_height < INC_POS_LOWER_LIMIT)
        current_pos++;
    data.map.current_pos = current_pos;
    return data; // TESTING

    path_step next = in.map.path[current_pos];

    int num_edges = 3; //in.g.get_edges(next.node).size();

    // Turning areas have more than 2 edges
    if (num_edges > 2) {
        data.out.angle = next.dir * 30;
        data.out.speed = 10;
        return data;
    }

    // Approaching a stop-line, is it the end node?
    if (next.node == in.map.path.back().node) {
        data.sys = stopping;
        data.out.speed = 5;
        data.dist = in.metrics.dist_stop_line;
        return data;
    }

    // A stop-line which is not the end-node.
    return data;
}

pid_decision_return regulate(pid_decision_data &dec) {

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

  line_in.params = dec.out.params;
  line_in.dt = dec.out.dt;
  pid_system_out line_out = pid_line(line_in);
  pid_decision_return out =
    {
     .angle = line_out.angle,
     .speed = line_out.speed,
     .samples = line_out.samples
    };
    queue_message("Hello");

  return out;
}
