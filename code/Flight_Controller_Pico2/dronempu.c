#include "dronempu.h"
#include <math.h>

// I2C address of MPU6050
#define MPU6050_ADDR 0x68

float RateRoll, RatePitch, RateYaw;
float calRateRoll, calRatePitch, calRateYaw;
float MeasRoll, MeasPitch;
float AccelX, AccelY, AccelZ; //in g not m/s^2
float calAccelX, calAccelY, calAccelZ; 

//filter stuff
float FilteredRoll = 0.0f;
float FilteredPitch = 0.0f;

float LPF_RateRoll= 0.0f;
float LPF_RatePitch = 0.0f;
float LPF_RateYaw =0.0f;


#define ACCEL_SENS 16384.0f   // LSB / g
#define PI 3.141592

void write_reg(uint8_t reg, uint8_t data) {
    uint8_t buf[2] = {reg, data};
    i2c_write_blocking(i2c0, MPU6050_ADDR, buf, 2, false);
}

void read_regs(uint8_t reg, uint8_t *buf, uint8_t len) {
    i2c_write_blocking(i2c0, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c0, MPU6050_ADDR, buf, len, false);
}

void gyro_signals(void) {
    uint8_t data[6];

    // Gyroscope configuration (same as Arduino sketch)
    write_reg(0x1A, 0x05);  // DLPF config
    write_reg(0x1B, 0x08);  // Gyro ±500°/s

    // Read gyro registers
    read_regs(0x43, data, 6);

    uint16_t GyroX = (data[0] << 8) | data[1];
    uint16_t GyroY = (data[2] << 8) | data[3];
    uint16_t GyroZ = (data[4] << 8) | data[5];

    // Convert to °/s (LSB sensitivity = 65.5 for ±500°/s)
    RateRoll  = GyroX / 65.5f;
    RatePitch = GyroY / 65.5f;
    RateYaw   = GyroZ / 65.5f;
}

void accel_raw(uint16_t *x, uint16_t *y, uint16_t *z) {
    uint8_t data[6];
    read_regs(0x3B, data, 6);

    *x = (data[0] << 8) | data[1];
    *y = (data[2] << 8) | data[3];
    *z = (data[4] << 8) | data[5];
}

void calibrate_sensors(void){
    //calibrate gyro and accel - sit so z perp to gravity 
    for(int i = 0; i < 2000; i++){ 
        gyro_signals(); 
        calRateRoll  += RateRoll; 
        calRatePitch += RatePitch; 
        calRateYaw   += RateYaw; 
        sleep_ms(1); 
    }
    calRateRoll  /= 2000.0f;
    calRatePitch /= 2000.0f;
    calRateYaw   /= 2000.0f;
    float sumX = 0, sumY = 0, sumZ = 0;
    for (int i = 0; i < 2000; i++) {
        accel_signals(); // raw read
        sumX += AccelX;
        sumY += AccelY;
        sumZ += AccelZ;
        sleep_ms(1);
    }
    float avgX = sumX / 2000.0f;
    float avgY = sumY / 2000.0f;
    float avgZ = sumZ / 2000.0f;

    calAccelX = avgX - 0.0f;
    calAccelY = avgY - 0.0f;
    calAccelZ = avgZ - 1.0f;   // should read +1g on Z 

}

void get_accel_angles(){ //NEED SOME SORT OF COMP FILTER OR KALMAN FILTER
    //spiking values while moving messed up pid controller
    //future implementation!

    //roll calc: arctan(y/z) [abt x axis]
    MeasRoll = atan2(AccelY, AccelZ) * 180.0f / PI;

    //pitch calc: arctan(-x/(sqrt(y^2+z^2)))
    MeasPitch = atan2(-AccelX, sqrt(AccelY*AccelY+AccelZ*AccelZ)) * 180.0f / PI;
}

void comp_filter_angle(float dt){
    //0.98 = trust gyro 98%
    //0.02 = trust accelerometer 2% (for long term correction)
    const float alpha = 0.98f;
    get_accel_angles();

    FilteredRoll = alpha *(FilteredRoll + (RateRoll-calRateRoll) *dt) + (1.0f - alpha) * MeasRoll;
    FilteredPitch = alpha *(FilteredPitch + (RatePitch-calRatePitch) *dt) + (1.0f - alpha) * MeasPitch;
}

void init_sensors(void){
    //init i2c at 40k
    i2c_init(i2c0, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);  // SDA
    gpio_set_function(5, GPIO_FUNC_I2C);  // SCL
    gpio_pull_up(4);
    gpio_pull_up(5);

    sleep_ms(250);

    // Wake up MPU6050
    write_reg(0x6B, 0x00);
}
void calibrate_gyro(void){
    float ALPHA = 0.3f;
    // Apply gyro calibration
    RateRoll -= calRateRoll;
    RatePitch -= calRatePitch;
    RateYaw -= calRateYaw;

    //Apply LPF on gyro calc
    LPF_RateRoll = ALPHA * RateRoll + (1.0f - ALPHA) * LPF_RateRoll;
    LPF_RatePitch = ALPHA * RatePitch + (1.0f - ALPHA) * LPF_RatePitch;
    LPF_RateYaw = ALPHA * RateYaw + (1.0f - ALPHA) * LPF_RateYaw;
}
void accel_signals(void) {
    uint8_t data[6];

    // Accel config: ±2g
    write_reg(0x1C, 0x00);

    read_regs(0x3B, data, 6);

    int16_t rawX = (data[0] << 8) | data[1];
    int16_t rawY = (data[2] << 8) | data[3];
    int16_t rawZ = (data[4] << 8) | data[5];

    // Convert to g units
    AccelX = (rawX / ACCEL_SENS) - calAccelX;
    AccelY = (rawY / ACCEL_SENS) - calAccelY;
    AccelZ = (rawZ / ACCEL_SENS) - calAccelZ;
}

float get_filtered_roll(void){
    return FilteredRoll;
}
float get_filtered_pitch(void){
    return FilteredPitch;
}
float get_LPF_roll(void){
    return LPF_RateRoll;
}
float get_LPF_pitch(void){
    return LPF_RatePitch;
}
float get_LPF_yaw(void){
    return LPF_RateYaw;
}

/*
int main() {
    stdio_init_all();

    //init i2c at 40k
    i2c_init(i2c0, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);  // SDA
    gpio_set_function(5, GPIO_FUNC_I2C);  // SCL
    gpio_pull_up(4);
    gpio_pull_up(5);

    sleep_ms(250);

    // Wake up MPU6050
    write_reg(0x6B, 0x00);

    //calibrate gyro and accel - sit so z perp to gravity 
    for(int i = 0; i < 2000; i++){ 
        gyro_signals(); 
        calRateRoll  += RateRoll; 
        calRatePitch += RatePitch; 
        calRateYaw   += RateYaw; 
        sleep_ms(1); 
    }
    calRateRoll  /= 2000.0f;
    calRatePitch /= 2000.0f;
    calRateYaw   /= 2000.0f;
    float sumX = 0, sumY = 0, sumZ = 0;
    for (int i = 0; i < 2000; i++) {
        accel_signals(); // raw read
        sumX += AccelX;
        sumY += AccelY;
        sumZ += AccelZ;
        sleep_ms(1);
    }
    float avgX = sumX / 2000.0f;
    float avgY = sumY / 2000.0f;
    float avgZ = sumZ / 2000.0f;

    calAccelX = avgX - 0.0f;
    calAccelY = avgY - 0.0f;
    calAccelZ = avgZ - 1.0f;   // should read +1g on Z 

    while (1) {
        gyro_signals();
        accel_signals();

        // Apply gyro calibration
        RateRoll  -= calRateRoll;
        RatePitch -= calRatePitch;
        RateYaw   -= calRateYaw;

        printf("Gyro: Roll %.2f  Pitch %.2f  Yaw %.2f\n",
               RateRoll, RatePitch, RateYaw);

        printf("Accel (g): X %.3f  Y %.3f  Z %.3f\n",
               AccelX, AccelY, AccelZ);

        sleep_ms(500);
    }
}

*/

//debugging test for main function stuff

//     uint8_t whoami_reg = 0x75;
//     uint8_t whoami;

//     while (1) {
//         if (i2c_write_blocking(i2c0, 0x68, &whoami_reg, 1, true) < 0) {
//             printf("I2C WRITE FAIL\n");
//         } else if (i2c_read_blocking(i2c0, 0x68, &whoami, 1, false) < 0) {
//             printf("I2C READ FAIL\n");
//         } else {
//             printf("WHOAMI = 0x%02X\n", whoami); //0x68 means running correctly
//         }
//         sleep_ms(500);
//     }
// }