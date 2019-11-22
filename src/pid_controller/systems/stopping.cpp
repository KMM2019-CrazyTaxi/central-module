#include "stopping.hpp"
#include "logging.hpp"

pid_system_out pid_stopping(const pid_decision_data &in) {

  double kp = in.params.stopping.kp;
  double ki = in.params.stopping.ki;
  double kd = in.params.stopping.kd;
  double alpha = in.params.stopping.alpha;
  double beta = in.params.stopping.beta;

  double dist = in.metrics.dist_stop_line;
  double speed = in.metrics.curr_speed;

  double calc_p = alpha * ref_speed_updated - metrics.curr_speed;
  double calc_i = 0;
  double calc_d = (sample_d - samples.line_speed_d) / dt;

  double p = kp * calc_p;
  double i = ki * calc_i;
  double d = kd * calc_d;

  samples.line_speed_d = sample_d;
  
  return out;
}
