#include <stdint.h>
#include "hardware/i2c.h"

#define IMU_SDA 6
#define IMU_SCL 7
#define IMU_I2C i2c1

extern double global_roll;
extern double global_pitch;
extern double global_yaw;

extern double global_accel_x;
extern double global_accel_y;
extern double global_accel_z;

void imu_init();
void imu_start_interrupts();
void imu_read();