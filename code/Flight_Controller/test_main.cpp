#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "timer.h"
#include "pid.h"
#include "controller.h"
#include "mixer.h"
#include "bar180f.h"        
#include "dshot_encoder.h" 
#include "dronempu.h"

#define GPIO_FORWARD_SIGNAL 22 // pin that triggers forward motion

//all globals here 
bmp180_t bmp;
//NEED ARRAY OF MOTOR FLOATS! FUCK DA CTRL AND MIX SHII
//controller_state_t ctrl;
//mixer_outputs_t mix;

//call the globals from mpu file -- might not be right



PIO pio = pio0;
DShotEncoder motorFL(pio, 12); // front-left
DShotEncoder motorFR(pio, 14);  // front-right
DShotEncoder motorBL(pio, 13);  // back-left
DShotEncoder motorBR(pio, 15);  // back-right
float motors[4];

//pull in PID vals?
extern PID roll_rate_pid;
extern PID pitch_rate_pid;
extern PID yaw_rate_pid;
extern PID roll_angle_pid;
extern PID pitch_angle_pid;
extern PID altitude_pid;



// --- TEST MODE SELECT ---
void print_menu()
{
    printf("\n=== DRONE TEST MENU ===\n");
    printf("1 - Test IMU (gyro + accel)\n");
    printf("2 - Test Barometer altitude\n");
    printf("3 - Test PID loops (no motors)\n");
    printf("4 - Test Mixer + Motors (NO PROPS!)\n");
    printf("==========================\n");
}

void test_imu()
{


    printf("IMU TEST STARTED\n");
    sleep_ms(500);
    // calibrate it and test
    //calibrate_sensors(); 
    while (1)
    {
        gyro_signals();
        accel_signals();
        get_accel_angles();

        // Apply gyro calibration
        RateRoll -= calRateRoll;
        RatePitch -= calRatePitch;
        RateYaw -= calRateYaw;

        printf("Gyro: Roll %.2f  Pitch %.2f  Yaw %.2f\n",
               RateRoll, RatePitch, RateYaw);

        printf("Calc: Roll Measured %.2f  Pitch %.2f \n",
               MeasRoll, MeasPitch);

        printf("Accel (g): X %.3f  Y %.3f  Z %.3f\n",
               AccelX, AccelY, AccelZ);

        sleep_ms(500);
    }
}

void test_baro()
{

    printf("BAROMETER TEST STARTED\n");

    bmp.oversample = 2;
    bmp.sealevel_pressure = 1013.25;

    while (1) {
        float tempC = bmp180_read_temperature(&bmp);
        float tempF = tempC * 9.0f / 5.0f + 32.0f;
        float pressure = bmp180_read_pressure_hPa(&bmp);
        float altitude = bmp180_read_altitude(&bmp);

        printf("%.2f °C  %.2f °F  %.2f hPa  %.2f m\n",
               tempC, tempF, pressure, altitude);

        sleep_ms(500);
    }
}

void test_pid()
{
    printf("PID TEST STARTED (NO MOTORS)\n");
    printf("Move the drone and watch printed PID corrections.\n");

    controller_init();

    uint64_t last = get_absolute_time(); //might have to be absolute time type

    while (1)
    {
        gyro_signals();
        accel_signals();
        get_accel_angles();
        RateRoll -= calRateRoll;
        RatePitch -= calRatePitch;
        RateYaw -= calRateYaw;
        float altitude = bmp180_read_altitude(&bmp);

        float dt = (float)absolute_time_diff_us(last, get_absolute_time()) / 1e6f;
        last = get_absolute_time();

        controller_update(dt, MeasRoll, MeasPitch, RateRoll, RatePitch, RateYaw, altitude, motors, 0 );

        //printf("Rate Changes: roll=%.2f pitch=%.2f yaw=%.2f",RateRoll, RatePitch, RateYaw);
        //printf(" | Measured Angles: roll_angle=%.2f pitch_angle=%.2f", MeasRoll, MeasPitch);
        printf(" | Motor output values: FR=%.2f FL=%.2f BR=%.2f BL=%.2f \n", motors[1], motors[3], motors[0], motors[2]);

        sleep_ms(500);
    }
}

void test_motors()
{
    
    printf("\nMOTOR MIXER / DSHOT TEST (NO PROPS!)\n");

    printf("Slowly tilting the drone should change motor outputs.\n");
    printf("Press CTRL+C to exit.\n");

    controller_init();
    uint64_t last = get_absolute_time(); //might have to be absolute time type

    while (1)
    {
        gyro_signals();
        accel_signals();
        get_accel_angles();
        RateRoll -= calRateRoll;
        RatePitch -= calRatePitch;
        RateYaw -= calRateYaw;
        float altitude = bmp180_read_altitude(&bmp);

        float dt = (float)absolute_time_diff_us(last, get_absolute_time()) / 1e6f;
        last = get_absolute_time();

        controller_update(dt, MeasRoll, MeasPitch, RateRoll, RatePitch, RateYaw, altitude, motors, 0 );

        // Send throttle to each motor
        motorFL.sendThrottle(motors[3]);
        motorFR.sendThrottle(motors[1]);
        motorBL.sendThrottle(motors[2]);
        motorBR.sendThrottle(motors[0]);

        //printf("Rate Changes: roll=%.2f pitch=%.2f yaw=%.2f",RateRoll, RatePitch, RateYaw);
        //printf(" | Measured Angles: roll_angle=%.2f pitch_angle=%.2f", MeasRoll, MeasPitch);
        printf(" | Motor output values: FR=%.2f FL=%.2f BR=%.2f BL=%.2f \n", motors[1], motors[3], motors[0], motors[2]);

        sleep_ms(20);
    }
    
}

int main()
{
    stdio_init_all();
    sleep_ms(1000);

    init_sensors(); //look into not doing it this way - i2c used by baro too
    calibrate_sensors();
    
    if (!bmp180_init(&bmp, i2c0)) {
        printf("BMP180 init failed!\n");
        return 1;
    }
    else {
       printf("bmp init");
    }
    // Init motors - make sure pins are right
    PIO pio = pio0;
    motorFL = DShotEncoder(pio, 12);
    motorFR = DShotEncoder(pio, 14);
    motorBL = DShotEncoder(pio, 13);
    motorBR = DShotEncoder(pio, 15);

    motorFL.init(true);
    motorFR.init(true);
    motorBL.init(true);
    motorBR.init(true);

    // Arm - send 0 throttle
    motorFL.sendThrottle(0.0f);
    motorFR.sendThrottle(0.0f);
    motorBL.sendThrottle(0.0f);
    motorBR.sendThrottle(0.0f);
    sleep_ms(1500);

    // GPIO forward signal
    gpio_init(GPIO_FORWARD_SIGNAL);
    gpio_set_dir(GPIO_FORWARD_SIGNAL, GPIO_IN);

    print_menu();

    while (1)
    {
        int c = getchar_timeout_us(0);
        if (c == '1')
            test_imu();
        if (c == '2')
            test_baro();
        if (c == '3')
            test_pid();
        if (c == '4')
            test_motors();

        sleep_ms(50);
    }
}
