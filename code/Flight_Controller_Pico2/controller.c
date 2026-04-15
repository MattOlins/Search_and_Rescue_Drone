#include "controller.h"
#include "mixer.h"
#include "dronempu.h"

// =======================
// PID declarations
// =======================
PID roll_rate_pid;
PID pitch_rate_pid;
PID yaw_rate_pid;
PID roll_angle_pid;
PID pitch_angle_pid;
PID altitude_pid;


#define FLY_HEIGHT -8
 //goal is 1m above current location so.. changes every time bc of it does!

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
    float P_rate = 0.17; //corrective thurst?
    float I_rate = 0.00; //long term drift 
    //^trying to set to zero initially bc having saturation issues
    float D_rate = 0.01; //dampens isolation
    float limit_rate = 0.05; //allows large changes on motor output
    
    float P_outer = 1.3; //correcting its angle to target
    float I_outer = 0.00; //corrects persistent angle offset 
    //^trying to set to zero initially bc having saturation issues
    float D_outer = 0.09; //still unsure what it does tbh
    float limit_outer = 0.05;

    float p_alt = 1.0;
    float I_alt = 0.00;
    float D_alt = 0.05;
    float limit_alt = 0.05;

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

void controller_update(float dt, float alt_meas, float *motors,int forward_signal)
{
    float FilteredRoll = get_filtered_roll();
    float FilteredPitch = get_filtered_pitch();
    float LPF_RateRoll = get_LPF_roll();
    float LPF_RatePitch = get_LPF_pitch();
    float LPF_RateYaw = get_LPF_yaw();
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
    float roll_rate_target  = pid_update(&roll_angle_pid,  target_roll  - FilteredRoll,  dt);
    float pitch_rate_target = pid_update(&pitch_angle_pid, target_pitch - FilteredPitch, dt);

    float roll_out  = pid_update(&roll_rate_pid,  roll_rate_target - LPF_RateRoll, dt);
    float pitch_out = pid_update(&pitch_rate_pid, pitch_rate_target - LPF_RatePitch, dt);
    float yaw_out   = pid_update(&yaw_rate_pid,   target_yaw_rate - LPF_RateYaw, dt);

    float alt_out = pid_update(&altitude_pid, target_altitude - alt_meas, dt);

    printf("Roll: target=%.2f meas=%.2f out=%.2f\n",roll_rate_target, LPF_RateRoll, roll_out);
    //printf("PID: roll=%.2f pitch=%.2f yaw=%.2f alt=%.2f\n", roll_out, pitch_out, yaw_out, alt_out);
    //debug print statments

    //send to motor mixer
    float throttle = 0.4 + alt_out; 
    if (throttle > 0.8f) throttle = 0.8f;  //again lower if cap at .75
    if (throttle < 0.0f) throttle = 0.0f;
    mixer_x_configuration(throttle, roll_out, pitch_out, yaw_out, motors); 
        //not working getting same numbers/no changes in motor outputs...
}
