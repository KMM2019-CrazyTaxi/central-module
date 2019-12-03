#include "stopping.hpp"
#include "logging.hpp"
#include <algorithm>

double calc_dist(const double &, const double &);

pid_system_out pid_stopping(const pid_decision_data &in) {

  double kp = in.out.params.stopping.kp;
  double ki = in.out.params.stopping.ki;
  double kd = in.out.params.stopping.kd;
  double alpha = in.out.params.stopping.alpha;
  double beta = in.out.params.stopping.beta;

  double dist = in.out.metrics.dist_stop_line;
  double curr_speed = in.out.metrics.curr_speed;
  double ref_speed = in.out.speed;
  double threshold = in.out.params.stopping.speed_threshold;

  regulator_sample_data samples = in.out.samples;
  double sample_d = beta * ref_speed - curr_speed;
  double dt = in.out.dt;

  double calc_p = alpha * ref_speed * calc_dist(dist, threshold) - curr_speed;
  double calc_i = 0;
  double calc_d = (sample_d - samples.stopping_speed_d) / dt;

  double p = kp * calc_p;
  double i = ki * calc_i;
  double d = kd * calc_d;

  samples.line_speed_d = sample_d;

  double res = p + i + d;

  pid_system_out out =
    {
     .angle = in.out.angle,
     .speed = curr_speed + res,
     .samples = samples
    };

  return out;
}

double calc_dist(const double &dist, const double &threshold) {
  return std::min(dist, threshold);
}
