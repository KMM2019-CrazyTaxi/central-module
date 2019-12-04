#include "control_decisions.hpp"
#include "logging.hpp"

pid_decision_data decide(pid_decision_in &);
pid_decision_return regulate(const pid_decision_data &);


pid_decision_return pid_decision(pid_decision_in &in) {
    pid_decision_data data = decide(in);
    pid_decision_return out = regulate(data);
    out.current_pos = data.current_pos;
    return out;
}

pid_decision_data decide(pid_decision_in &in) {

    // Default settings for line following
    pid_decision_data data =
        {
        .sys = line,
        .current_pos = in.current_pos,
        .out.metrics = in.metrics,
        .out.params = in.params,
        .out.samples = in.samples,
        .out.samples.dist_stop_line = in.metrics.dist_stop_line,
        .out.dt = in.dt,
        .out.angle = 0,
        .out.speed = 10
        };
    return data; // TESTING

    // If the next stop line is far away, return line follower
    //if (in.metrics.dist_stop_line > 10) return data;

    int current_pos = in.current_pos;

    // If distance to stop line increased, we assume we passed one.
    // This also means that the initial sample value should be big.
    if (in.metrics.dist_stop_line > in.samples.dist_stop_line) current_pos++;
    data.current_pos = current_pos;

    path_step next = in.path[current_pos];

    int num_edges = 3; //in.g.get_edges(next.node).size();

    // Turning areas have more than 2 edges
    if (num_edges > 2) {
        double offset = 0;
        data.sys = turning;
        if (in.metrics.dist_left > 18)
            offset += 10;
        if (in.metrics.dist_right > 18)
            offset -= 10;

        data.out.angle = offset; //next.dir * 30 + offset;
        data.out.speed = 10;
        return data;
    }

    // Approaching a stop-line, is it the end node?
    if (next.node == in.path.back().node) {
        data.sys = stopping;
        data.out.speed = 5;
        return data;
    }

    // A stop-line which is not the end-node.
    return data;

}

pid_decision_return regulate(const pid_decision_data &dec) {

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

  line_in.dt = dec.out.dt;
  pid_system_out line_out = pid_line(line_in);
  pid_decision_return out =
    {
     .angle = line_out.angle,
     .speed = line_out.speed,
     .samples = line_out.samples
    };

  return out;
}
