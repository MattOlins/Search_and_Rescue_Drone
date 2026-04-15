#include <stdio.h>
#include "pico/stdlib.h"
#include "controller.h"
#include "mixer.h"

//including mpu and baro libs
#include "bar180f.h"
#include "dronempu.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "pico/stdlib.h"

#include "dshot_encoder.h"

#define LOOP_DT 0.01f   // 10 ms loop
#define FORWARD_GPIO 16 //decided which pin we are using here!
float motors[4];

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    // GPIO pin for flight trigger
    gpio_init(FORWARD_GPIO);
    gpio_set_dir(FORWARD_GPIO, GPIO_IN);

    controller_init();

    //motors
    PIO pio = pio0;
    DShotEncoder motor1(pio, 15);//back right CCW
    DShotEncoder motor2(pio, 14);//front right CW
    DShotEncoder motor3(pio, 13);//back left CW
    DShotEncoder motor4(pio, 12);//front left CCW

    motor1.init(true);
    motor2.init(true);
    motor3.init(true);
    motor4.init(true);

    motor1.sendThrottle(0);
    motor2.sendThrottle(0);
    motor3.sendThrottle(0);
    motor4.sendThrottle(0);
    sleep_ms(2000);

    // Main loop
    while (1)
    {
        //prolly need to calibrate it???
        float roll, pitch, yaw_rate;
        float altitude;

        mpu_read(&roll, &pitch, &yaw_rate); //mpu read?????
        altitude = baro_get_altitude(); //need to make pretty -fix duh

        float motors[4];

        int forward_signal = gpio_get(FORWARD_GPIO);

        controller_update(LOOP_DT, roll, pitch, yaw_rate, altitude, motors, forward_signal);

        motor1.sendThrottle(motors[0]);
        motor2.sendThrottle(motors[1]);
        
        motor3.sendThrottle(motors[2]);
        motor4.sendThrottle(motors[3]);

        sleep_ms(10);
    }
}
