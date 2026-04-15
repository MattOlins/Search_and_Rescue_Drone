#ifndef DRONEMPU_H
#define DRONEMPU_H


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#ifdef __cplusplus
extern "C" {
#endif

// declarations

extern float RateRoll, RatePitch, RateYaw;
extern float calRateRoll, calRatePitch, calRateYaw;
extern float MeasRoll, MeasPitch;
extern float AccelX, AccelY, AccelZ; //in g not m/s^2
extern float calAccelX, calAccelY, calAccelZ; 
//filtered signals
extern float FilteredRoll, FilteredPitch;
extern float LPF_RateRoll, LPF_RatePitch, LPF_RateYaw;

void write_reg(uint8_t reg, uint8_t data);

void read_regs(uint8_t reg, uint8_t *buf, uint8_t len);

void gyro_signals(void);

void accel_raw(uint16_t *x, uint16_t *y, uint16_t *z);

void calibrate_sensors(void);

void init_sensors(void);

void accel_signals(void);

void get_accel_angles(void);

void comp_filter_angle(float dt);

void calibrate_gyro(void);

float get_filtered_roll(void);
float get_filtered_pitch(void);
float get_LPF_roll(void);
float get_LPF_pitch(void);
float get_LPF_yaw(void);


#ifdef __cplusplus
}
#endif

#endif