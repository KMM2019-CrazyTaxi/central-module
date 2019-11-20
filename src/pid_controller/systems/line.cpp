#include "line.hpp"
#include "logging.hpp"

pid_system_out pid_line(const pid_system_out &in){
  // TODO: Implement proper behaviour
  pid_system_out out =
    {
     .angle = in.angle,
     .speed = (uint8_t)(in.speed + 1)
    };
  return out;
}
