#ifndef CM_PID_DECISIONS_H
#define CM_PID_DECISIONS_H

#include "regulator_data.hpp"
#include "turning.hpp"
#include "parking.hpp"
#include "stopping.hpp"
#include "line.hpp"

#include "pathfinding.hpp"
#include "graph.hpp"
#include "image_recognition_constants.hpp"

#define INC_POS_ERROR_DELTA IMAGE_HEIGHT/10
#define INC_POS_LOWER_LIMIT IMAGE_HEIGHT/8
#define INC_POS_UPPER_LIMIT IMAGE_HEIGHT - INC_POS_ERROR_DELTA

/**
  * This system makes control decisions and sends it forward to the next
  * appropriate sub-system.
  */
pid_decision_return pid_decision(pid_decision_in &in);

#endif
