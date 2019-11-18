#include "pid_controller.hpp"
#include "logging.hpp"

ctrl_output pid_ctrl_thread_main(ctrl_metrics metrics){

  ctrl_turn_metrics turn_metrics =
    {
     .current_turn = metrics.curr_turn;
     .input_turn = metrics.input_turn;
     .dist_left = metrics.dist_left;
     .dist_right = metrics.dist_right;
    };

  uint8_t regulated_turn = regulate_turn(turn_metrics);

  ctrl_speed_metrics speed_metrics =
    {
     .curr_speed = metrics.curr_speed;
     .input_speed = metrics.input_speed;
     .turn = regulated_turn;
    };

  uint8_t regulated_speed = regulate_speed(speed_metrics);

  ctrl_output output =
    {
     .speed = regulated_speed;
     .turn = regulated_turn;
    };

  return output;
}

uint8_t regulate_turn(ctrl_turn_metrics metrics) {
  
}

uint8_t regulate_speed(ctrl_speed_metrics metrics) {
  
}


