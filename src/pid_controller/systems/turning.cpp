#include "turning.hpp"
#include "logging.hpp"

pid_system_out pid_turning(const pid_decision_data &in) {

  /* For the moment, this is basically identical to the line follower's
     angle regulator. This can be tweaked in the future if necessary. */


  double kp = in.out.params.turning.kp;
  double ki = in.out.params.turning.ki;
  double kd = in.out.params.turning.kd;
  double alpha = in.out.params.turning.alpha;
  double beta = in.out.params.turning.beta;

  double ref_angle = in.out.angle;

  double dist_left = in.out.metrics.dist_left;
  double dist_right = in.out.metrics.dist_right;
  double diff = dist_left - dist_right;

  double dt = in.out.dt;
  regulator_sample_data samples = in.out.samples;
  double sample_d = beta * ref_angle - diff;

  double calc_p = alpha * ref_angle - diff;
  double calc_i = 0;
  double calc_d = (sample_d - samples.line_angle_d) / dt;

  double p = kp * calc_p;
  double i = ki * calc_i;
  double d = kd * calc_d;

  samples.line_angle_d = sample_d;

  double res = p + i + d;

  pid_system_out out =
    {
     .angle = res,
     .speed = in.out.speed,
     .samples = samples
    };

  return out;
}
