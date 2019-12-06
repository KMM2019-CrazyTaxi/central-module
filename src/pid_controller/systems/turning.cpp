#include "turning.hpp"
#include "logging.hpp"
#include <bitset>

pid_system_out pid_turning(pid_decision_data &in) {

    double kp = in.out.params.turning.kp;
    double ki = in.out.params.turning.ki;
    double kd = in.out.params.turning.kd;
    double alpha = in.out.params.turning.alpha;
    double beta = in.out.params.turning.beta;
    double min_value = in.out.params.turning.min_value;

    double ref_angle = in.out.angle;

    double dist_left = in.out.metrics.dist_left;
    double dist_right = in.out.metrics.dist_right;
    double diff = -dist_right + 10;

    /*
    // Depending on the look of the crossing, we want to follow different lines
    path_step next = in.map.path[in.map.current_pos];
    std::vector<edge> edges = in.map.g.get_edges(next.node);
    switch (next.dir) {

    // Going straight through crossing
    case STRAIGHT:
    {
        std::bitset<NUM_DIRECTIONS> dirs;

        for (edge e : edges)
            dirs.set(e.dir);

        // 4-way crossing
        if (dirs.test(LEFT) && dirs.test(RIGHT))
            diff = dist_left - dist_right;
        // 3-way crossing with a turn to the left
        else if (dirs.test(LEFT))
            diff = -dist_right + min_value;
        // 3-way crossing with a turn to the right
        else if (dirs.test(RIGHT))
            diff = dist_left - min_value;
        else
            queue_message("No directions are set in turning regulator");
    }break;

    // Turning left in crossing
    case LEFT:
    {
        diff = dist_left - min_value;

    }break;

    // Turning right in crossing
    case RIGHT:
    {
        diff = -dist_right + min_value;

    }break;

    default:
    {
        queue_message("Turning regulator got an invalid direction value" +
                std::to_string(next.dir));
    }break;

    }
    */

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
