#include "controller.h"
#include "mixer.h"

// =======================
// PID declarations
// =======================
PID roll_rate_pid;
PID pitch_rate_pid;
PID yaw_rate_pid;
PID roll_angle_pid;
PID pitch_angle_pid;
PID altitude_pid;

#define FLY_HEIGHT 37
 //goal is 1m above current location so.. 35.5+1

float hover_throttle = 0.2;
float target_altitude = FLY_HEIGHT;
float target_roll = 0.0f;
float target_pitch = 0.0f;
float target_yaw_rate = 0.0f;
int flight_mode = 0; //fsm variable
//0 is hover
//1 is fly forward
void controller_init()
{
    //PID tuning stuff - honestly no clue what these vals should be...
    //looking online these are the best starting points I found
    float P_rate = 0.3; //corrective thurst?
    float I_rate = 0.05; //long term drift 
    //^trying to set to zero initially bc having saturation issues
    float D_rate = 0.02; //dampens isolation
    float limit_rate = 0.3; //allows large changes on motor output

    float P_outer = 2.0; //correcting its angle to target
    float I_outer = 0.1; //corrects persistent angle offset 
    //^trying to set to zero initially bc having saturation issues
    float D_outer = 0.5; //still unsure what it does tbh
    float limit_outer = 0.2;

    float p_alt = 1.0;
    float I_alt = 0.1;
    float D_alt = 0.05;
    float limit_alt = 0.15;

    // // Rate PIDs
    pid_init(&roll_rate_pid,  P_rate, I_rate, D_rate, limit_rate);
    pid_init(&pitch_rate_pid, P_rate, I_rate, D_rate, limit_rate);
    pid_init(&yaw_rate_pid,   P_rate, I_rate, D_rate, limit_rate);

    // // Angle PIDs (outer loop)
    pid_init(&roll_angle_pid, P_outer, I_outer, D_outer, limit_outer);
    pid_init(&pitch_angle_pid, P_outer, I_outer, D_outer, limit_outer);

    // // Altitude PID
    pid_init(&altitude_pid, p_alt, I_alt, D_alt, limit_alt);
}

//we meed rate of roll, pitch, and yaw as well as the measured angles of all 3
    //we curently only using the rate not the angles

void controller_update(float dt,float roll_angle_meas,float pitch_angle_meas, 
    float roll_rate_meas,float pitch_rate_meas, float yaw_rate_meas,float alt_meas,
    float *motors,int forward_signal)
{
    if (forward_signal)
        flight_mode = 1;
    else
        flight_mode = 0;   // return to hover mode

    //hover state
    if (flight_mode == 0)
    {
        target_pitch = 0.0f;
        target_roll  = 0.0f;
        target_altitude = FLY_HEIGHT;
    }

    //forward state
    else if (flight_mode == 1)
    {
        target_pitch = -8.0f;   // tilt forward
        target_altitude = FLY_HEIGHT; // hold altitude
    }

    //update pids
    float roll_rate_target  = pid_update(&roll_angle_pid,  target_roll  - roll_angle_meas,  dt);
    float pitch_rate_target = pid_update(&pitch_angle_pid, target_pitch - pitch_angle_meas, dt);

    float roll_out  = pid_update(&roll_rate_pid,  roll_rate_target - roll_rate_meas, dt);
    float pitch_out = pid_update(&pitch_rate_pid, pitch_rate_target - pitch_rate_meas, dt);
    float yaw_out   = pid_update(&yaw_rate_pid,   target_yaw_rate - yaw_rate_meas, dt);

    float alt_out = pid_update(&altitude_pid, target_altitude - alt_meas, dt);

    printf("Roll: target=%.2f meas=%.2f out=%.2f\n",roll_rate_target, roll_rate_meas, roll_out);
    //printf("PID: roll=%.2f pitch=%.2f yaw=%.2f alt=%.2f\n", roll_out, pitch_out, yaw_out, alt_out);
    //debug print statments

    //send to motor mixer
    float throttle = 0.2 + alt_out; 
    mixer_x_configuration(throttle, roll_out, pitch_out, yaw_out, motors); 
        //not working getting same numbers/no changes in motor outputs...
}
