#include "control_decisions.hpp"
#include "logging.hpp"

pid_decision_data decide(pid_decision_in &);
pid_decision_return regulate(pid_decision_data &);


pid_decision_return pid_decision(pid_decision_in &in) {
    pid_decision_data data = decide(in);

    queue_message("Path:");
    std::string path_str;
    double curr_line_height = in.metrics.dist_stop_line;
    for (const path_step& step : data.map.path) {
        path_str += "N: " + std::to_string(step.node) + " D: " + std::to_string(step.dir) + " ";
    }
    queue_message(std::move(path_str));
    queue_message("Current system: " + std::to_string(data.sys));
    queue_message("CURR_LINE_HEIGHT: " + std::to_string(curr_line_height));
    queue_message("Node index: " + std::to_string(data.map.index));
    queue_message("Next node: " + std::to_string(data.map.path[data.map.index].node));

    pid_decision_return out = regulate(data);
    out.previous_pos = data.map.previous_pos;
    out.next_pos = data.map.next_pos;
    out.index = data.map.index;
    return out;
}

pid_decision_data decide(pid_decision_in &in) {

    // Default settings for line following
    pid_decision_data data =
        {
        .sys = angle,
        .map = in.map,
        .out.metrics = in.metrics,
        .out.params = in.params,
        .out.samples = in.samples,
        .out.samples.dist_stop_line = in.metrics.dist_stop_line,
        .out.dt = in.dt,
        .out.angle = 0,
        .out.speed = 10
        };

    // If an obstacle is ahead, we stop
    if (static_cast<double>(in.sensor_data.dist) < in.params.stopping.min_value) {
        data.sys = stopping;
        data.out.speed = 0;
        data.dist = (double)in.sensor_data.dist;
        return data;
    }

    // If distance to stop line increased, we assume we passed one.
    // This also means that the initial sample value should be big.
    double curr_line_height = in.metrics.dist_stop_line;
    double prev_line_height = in.samples.dist_stop_line;

    int index = in.map.index;
    if (curr_line_height > prev_line_height + INC_POS_ERROR_DELTA &&
            prev_line_height < INC_POS_LOWER_LIMIT &&
            curr_line_height > INC_POS_LOWER_LIMIT)
    {
        data.map.previous_pos = in.map.path[index].node;
        index++;
    }
    data.map.next_pos = in.map.path[index].node;
    data.map.index = index;

    path_step next = in.map.path[index];

    // Approaching a stop-line, is it the end node?
    if (next.node == in.map.path.back().node && curr_line_height < in.params.stopping.min_value) {
        data.sys = stopping;
        data.out.speed = 0;
        data.dist = in.metrics.dist_stop_line;
        data.set_if_finished = true;
        return data;
    }

    uint8_t num_edges = in.map.g.get_edges(data.map.previous_pos).size();
    // Turning areas have more than 1 edge
    if (num_edges > 1) {
        data.sys = turning;
        return data;
    }

    // A stop-line which is not the end-node.
    return data;
}

pid_decision_return regulate(pid_decision_data &dec) {

  pid_system_out reg_angle = dec.out;

  switch(dec.sys) {

  case turning:
    {
      reg_angle = pid_turning(dec);

    }break;

  case parking:
    {
      reg_angle = pid_parking(dec);

    }break;

  case stopping:
    {
//      reg_angle = pid_stopping(dec);
        pid_system_out stop = pid_stopping(Dec);
        reg_angle = pid_angle(dec);
        reg_angle.speed = stop.speed;

    }break;

  case angle:
    {
      reg_angle = pid_angle(dec);

    }break;

  default:
    {
      queue_message("Regulator sent a request for a non-existing system: " + std::to_string(dec.sys));
    }break;

  }

  reg_angle.dt = dec.out.dt;
  reg_angle.params = dec.out.params;
  reg_angle.metrics = dec.out.metrics;
  pid_system_out speed = pid_speed(reg_angle);
  pid_decision_return out =
    {
     .angle = reg_angle.angle,
     .speed = speed.speed,
     .samples = speed.samples,
     .mission_finished = reg_angle.mission_finished
    };

  return out;
}
