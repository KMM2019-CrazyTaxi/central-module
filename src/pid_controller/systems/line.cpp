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

pid_system_out pid_line(pid_system_out &in){
  // TODO: Implement proper behaviour

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

  double fact = (1 - min_fact) / (90 * min_fact) * reg_angle;
  // 1 when reg_angle = 0, min_fact% of 1 when reg_angle = +- 90
  double speed_fact;
  if (reg_angle >= 0) speed_fact = fact + 1;
  else speed_fact = -fact + 1;
  double ref_speed_updated = ref_speed * speed_fact ;
  
  double sample_d = beta * ref_speed_updated - metrics.curr_speed;

  double calc_p = alpha * ref_speed_updated - metrics.curr_speed;
  double calc_i = 0;
  double calc_d = (sample_d - samples.line_speed_d) / dt;

  double p = kp * calc_p; // @TODO:Should the speed_fact be elsewhere?
  double i = ki * calc_i;
  double d = kd * calc_d;

  samples.line_speed_d = sample_d;
  
  return metrics.curr_speed + p + i + d;
}
