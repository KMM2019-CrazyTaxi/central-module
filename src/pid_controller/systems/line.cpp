#include "line.hpp"
#include "logging.hpp"
#include <cstdint>

double regulate_angle(const telemetrics_data &,
		      const pid_params &,
		      const double &,
		      const double &,
		      regulator_sample_data &);

double regulate_speed(const telemetrics_data &,
		      const pid_params &,
		      const double &,
		      const double &,
		      const double &,
		      regulator_sample_data &);

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

pid_system_out pid_line(pid_system_out &in){

  double reg_angle = regulate_angle(in.metrics,
				    in.params.line_angle,
				    in.angle,
				    in.dt,
				    in.samples);
  double reg_speed = regulate_speed(in.metrics,
				    in.params.line_speed,
				    in.speed,
				    reg_angle,
				    in.dt,
				    in.samples);
  
  pid_system_out out =
    {
     .angle = reg_angle,
     .speed = reg_speed,
     .samples = in.samples
    };

  return out;
}

double regulate_angle(const telemetrics_data &metrics,
		      const pid_params &params,
		      const double &ref_angle,
		      const double &dt,
		      regulator_sample_data &samples){

  double kp = params.kp;
  double ki = params.ki;
  double kd = params.kd;
  double alpha = params.alpha;
  double beta = params.beta;
  
  double dist_left = metrics.dist_left;
  double dist_right = metrics.dist_right;
  double diff = dist_left - dist_right;
  double sample_d = beta * ref_angle - diff;

  double calc_p = alpha * ref_angle - diff;
  double calc_i = 0;
  double calc_d = (sample_d - samples.line_angle_d) / dt;

  double p = kp * calc_p;
  double i = ki * calc_i;
  double d = kd * calc_d;

  samples.line_angle_d = sample_d;
  
  return p + i + d;
}

double regulate_speed(const telemetrics_data &metrics,
		      const pid_params &params,
		      const double &ref_speed,
		      const double &reg_angle,
		      const double &dt,
		      regulator_sample_data &samples){

  double min_fact = 10; // @TODO: SEND THIS VIA PARAMS OR SET CONSTANT?

  double kp = params.kp;
  double ki = params.ki;
  double kd = params.kd;
  double alpha = params.alpha;
  double beta = params.beta;

  double angle_threshold = params.angle_threshold;
  double speed_threshold = params.speed_threshold;
  double min_value = params.min_value;
  double slope = params.slope;

  double speed_fact = calc_fact(reg_angle, ref_speed, angle_threshold, speed_threshold, min_value, slope);
  double ref_speed_updated = ref_speed * speed_fact ;
  
  double sample_d = beta * ref_speed_updated - metrics.curr_speed;

  double calc_p = alpha * ref_speed_updated - metrics.curr_speed;
  double calc_i = 0;
  double calc_d = (sample_d - samples.line_speed_d) / dt;

  double p = kp * calc_p;
  double i = ki * calc_i;
  double d = kd * calc_d;

  samples.line_speed_d = sample_d;
  
  return metrics.curr_speed + p + i + d;
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

  double exp = slope^(-cutoff_func(reg_angle, ref_speed, angle_threshold, speed_threshold));
  double linear = cutoff_func(reg_angle, ref_speed, angle_threshold, speed_threshold) * (slope(-1) - min_value);

  return exp - linear;
}

/**
 * Will return 0 if |x| or |y| is within a threshold, or otherwise a linear function
 */
double cutoff_func(const double &x,
		   const double &y,
		   const double &c1,
		   const double &c2) {
  
  if (abs(x) < c1 || abs(y) < c2)
    return 0;
  else
    return abs((abs(x) - c1) * (abs(y) - c2)/((1 - c1) * (1 - c2)));
}
