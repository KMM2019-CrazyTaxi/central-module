#include "logging.hpp"
#include "speed.hpp"
#include <cstdint>
#include <cmath>
#include "math.h"

double calc_fact(const double &,
         const double &,
         const double &,
         const double &,
         const double &,
         const double &);

double cutoff_func(const double &,
           const double &,
           const double &,
           const double &);


pid_system_out pid_speed(pid_system_out &in){

  double kp = in.params.line_speed.kp;
  double ki = in.params.line_speed.ki;
  double kd = in.params.line_speed.kd;
  double alpha = in.params.line_speed.alpha;
  double beta = in.params.line_speed.beta;

  double angle_threshold = in.params.line_speed.angle_threshold;
  double speed_threshold = in.params.line_speed.speed_threshold;
  double min_value = in.params.line_speed.min_value;
  double slope = in.params.line_speed.slope;

  telemetrics_data metrics = in.metrics;

  double reg_angle = in.angle;
  double ref_speed = in.speed;
  double curr_speed = in.metrics.curr_speed;

  double speed_fact = calc_fact(reg_angle/MAX_INPUT_ANGLE,
          curr_speed/6, angle_threshold/MAX_INPUT_ANGLE,
          speed_threshold/6, min_value, slope);

  queue_message("Curr speed: " + std::to_string(metrics.curr_speed));
  queue_message("Reg angle: " + std::to_string(reg_angle));
  queue_message("Speed fact: " + std::to_string(speed_fact));

  double sample_d = beta * ref_speed - metrics.curr_speed;

  double calc_p = alpha * ref_speed - metrics.curr_speed;
  double calc_i = 0;
  double calc_d = (sample_d - in.samples.line_speed_d) / in.dt;

  double p = kp * calc_p;
  double i = ki * calc_i;
  double d = kd * calc_d;

  in.samples.line_speed_d = sample_d;

  double res = (metrics.curr_speed + p + i + d) * speed_fact;

  pid_system_out out =
  {
      .angle = in.angle,
      .speed = res,
      .samples = in.samples
  };
}

/**
 * Calculates a factor for the speed between min_value and 1.
 * Will always be 1 if |reg_angle| < angle_threshold or
 * if |ref_speed| < speed_threshold. Otherwise calculates an appropriate factor based on slope.
 *
 */
double calc_fact(const double &reg_angle,
         const double &ref_speed,
         const double &angle_threshold,
         const double &speed_threshold,
         const double &min_value,
         const double &slope) {

  if (slope < 1)
    queue_message("The slope of the calc_fact should never be < 1. Given: " + std::to_string(slope));

  double exp = pow(slope, -cutoff_func(reg_angle, ref_speed,
              angle_threshold, speed_threshold));
  double linear = cutoff_func(reg_angle, ref_speed,
          angle_threshold, speed_threshold) * (pow(slope, -1) - min_value);
  return exp - linear;
}

/**
 * Will return 0 if |x| or |y| is within a threshold, or otherwise a linear function
 */
double cutoff_func(const double &x,
           const double &y,
           const double &c1,
           const double &c2) {

  if (fabs(x) < c1 || fabs(y) < c2)
    return 0;
  else
    return fabs((fabs(x) - c1) * (fabs(y) - c2)/((1 - c1) * (1 - c2)));
}
