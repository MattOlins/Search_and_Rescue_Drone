#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "pid.h"
#include <stdio.h>
#include "pico/stdlib.h"

// Attitude rate controllers
extern PID roll_rate_pid;
extern PID pitch_rate_pid;
extern PID yaw_rate_pid;

// Attitude outer loop controllers
extern PID roll_angle_pid;
extern PID pitch_angle_pid;

// Altitude controller
extern PID altitude_pid;

#ifdef __cplusplus
extern "C" {
#endif

void controller_init();
void controller_update(
    float dt,
    float roll_angle_meas,
    float pitch_angle_meas,
    float roll_rate_meas,
    float pitch_rate_meas,
    float yaw_rate_meas,
    float altitude_meas,
    float *motor_outputs,
    int forward_signal
);

#ifdef __cplusplus
}
#endif

#endif

// Public control functions
