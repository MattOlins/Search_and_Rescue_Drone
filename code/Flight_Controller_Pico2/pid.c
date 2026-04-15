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
    //clamp so no windup error
    float max_integral = pid->output_limit / pid->ki;  // If ki > 0
    if (pid->integrator > max_integral) pid->integrator = max_integral;
    if (pid->integrator < -max_integral) pid->integrator = -max_integral;

    float i = pid->ki * pid->integrator;


    float d = 0;
    if (dt > 0.0001f) {  //just to avoid div by zero :)
        d = pid->kd * (error - pid->prev_error) / dt;
    }
    pid->prev_error = error;

    float out = p + i + d;

    //clamping
    if (out > pid->output_limit) out = pid->output_limit;
    if (out < -pid->output_limit) out = -pid->output_limit;

    return out;
}
