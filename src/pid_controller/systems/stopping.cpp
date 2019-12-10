#include "stopping.hpp"
#include "logging.hpp"
#include <cmath>

pid_system_out pid_stopping(const pid_decision_data &in) {

    double dist = std::min(in.out.metrics.dist_stop_line, static_cast<double>(in.out.sensor_data.dist));
    double curr_speed = in.out.metrics.curr_speed;
    double speed_cutoff = in.out.params.stopping.speed_threshold;

    double res = 0;


    bool finished = false;
    // If we are going too fast, break
    if (curr_speed > speed_cutoff) res = -curr_speed;
    // We are setting speed to 0, should mean we might have finished
    else if (in.set_if_finished) finished = true;

    pid_system_out out =
      {
       .angle = in.out.angle,
       .speed = res,
       .samples = in.out.samples,
       .mission_finished = finished
      };

    return out;
}
