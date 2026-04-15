#include "pid.h"

void pid_init(PID *pid, float kp, float ki, float kd, float limit)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integrator = 0;
    pid->prev_error = 0;
    pid->output_limit = limit;
}

float pid_update(PID *pid, float error, float dt)
{
    float p = pid->kp * error;

    pid->integrator += error * dt;
    float i = pid->ki * pid->integrator;

    float d = pid->kd * (error - pid->prev_error) / dt;
    pid->prev_error = error;

    float out = p + i + d;

    if (out > pid->output_limit) out = pid->output_limit;
    if (out < -pid->output_limit) out = -pid->output_limit;

    return out;
}
