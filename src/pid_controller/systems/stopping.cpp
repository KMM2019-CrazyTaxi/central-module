#include "stopping.hpp"
#include "logging.hpp"

double calc_ref_speed(double, double, double, double);

pid_system_out pid_stopping(const pid_decision_data &in) {

  double kp = in.out.params.stopping.kp;
  double ki = in.out.params.stopping.ki;
  double kd = in.out.params.stopping.kd;
  double alpha = in.out.params.stopping.alpha;
  double beta = in.out.params.stopping.beta;

  double dist = in.out.metrics.dist_stop_line;
  double curr_speed = in.out.metrics.curr_speed;
  double ref_speed = in.out.speed;
  double thold = in.out.params.stopping.speed_threshold;
  double min_dist = in.out.params.stopping.min_value;

  double ref_speed_updated = calc_ref_speed(ref_speed, dist, min_dist, thold);

  regulator_sample_data samples = in.out.samples;
  double sample_d = beta * ref_speed_updated - curr_speed;
  double dt = in.out.dt;

  double calc_p = alpha * ref_speed_updated - curr_speed;
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

double calc_ref_speed(double ref_speed, double dist,
                        double min_dist, double threshold) {
    if (dist > threshold) return ref_speed;
    return (ref_speed / (threshold - min_dist)) * (dist - min_dist);
}
