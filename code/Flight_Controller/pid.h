#ifndef PID_H
#define PID_H

typedef struct {
    float kp;
    float ki;
    float kd;
    float integrator;
    float prev_error;
    float output_limit;
} PID;

void pid_init(PID *pid, float kp, float ki, float kd, float limit);
float pid_update(PID *pid, float error, float dt);

#endif
