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
        .sys = line,
        .map.g = in.map.g,
        .map.path = in.map.path,
        .map.previous_pos = in.map.previous_pos,
        .map.next_pos = in.map.next_pos,
        .map.index = in.map.index,
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

  pid_system_out output;

  switch(dec.sys) {

  case turning:
    {
      pid_system_out output = pid_turning(dec);

    }break;

  case parking:
    {
      pid_system_out output = pid_parking(dec);

    }break;

  case stopping:
    {
      pid_system_out output = pid_stopping(dec);

    }break;

  case line:
    {
      output = pid_line(dec.out);

    }break;

  default:
    {
      queue_message("Regulator sent a request for a non-existing system: " + std::to_string(dec.sys));
    }break;

  }

  pid_decision_return out =
    {
     .angle = output.angle,
     .speed = output.speed,
     .samples = output.samples,
     .mission_finished = output.mission_finished
    };

  return out;
}
