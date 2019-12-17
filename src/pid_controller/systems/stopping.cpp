#include "stopping.hpp"
#include "logging.hpp"
#include <cmath>

double last_dist{ 10000 };
const double EPS{  };

pid_system_out pid_stopping(const pid_decision_data &in) {

    double dist = in.dist;
    double curr_speed = in.out.metrics.curr_speed;
    double speed_cutoff = in.out.params.stopping.speed_threshold;
    double res = 0;


    bool finished = false;
    // If we are going too fast, break
    if (last_dist < 300 && last_dist < dist - EPS)
    {
        finished = in.set_if_finished;
        last_dist = 10000;
    }
    else if (curr_speed > speed_cutoff)
    {
        res = -curr_speed;
        last_dist = dist;
    }
    // We are setting speed to 0, should mean we might have finished
    else if (in.set_if_finished)
    {
        last_dist = 10000;
        finished = true;
    }

    pid_system_out out =
      {
       .angle = in.out.angle,
       .speed = res,
       .samples = in.out.samples,
       .mission_finished = finished
      };

    return out;
}
