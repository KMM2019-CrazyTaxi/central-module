#include "angle.hpp"
#include "logging.hpp"

pid_system_out pid_angle(pid_decision_data &in){

  double kp = in.out.params.line_angle.kp;
  double ki = in.out.params.line_angle.ki;
  double kd = in.out.params.line_angle.kd;
  double alpha = in.out.params.line_angle.alpha;
  double beta = in.out.params.line_angle.beta;
  double offset = in.out.param.line_angle.min_value;

  double ref_angle = in.out.angle;
  double dist_left = in.out.metrics.dist_left;
  double dist_right = in.out.metrics.dist_right;
  double diff = dist_left - dist_right - offset;
  double sample_d = beta * ref_angle - diff;

  double calc_p = alpha * ref_angle - diff;
  double calc_i = 0;
  double calc_d = (sample_d - in.out.samples.line_angle_d) / in.out.dt;

  double p = kp * calc_p;
  double i = ki * calc_i;
  double d = kd * calc_d;

  in.out.samples.line_angle_d = sample_d;

  double res = p + i + d;

  pid_system_out out =
    {
     .angle = res,
     .speed = in.out.speed,
     .samples = in.out.samples
    };

  return out;
}

