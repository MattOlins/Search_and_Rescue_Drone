#include <stdio.h>
#include "pico/stdlib.h"
#include "timer.h"
#include "dshot_encoder.h"

int main()
{
    stdio_init_all();
    sleep_ms(2000); // wait 2 sec for esc to power on any or anything else that needs to happen


    /*
    MOTOR CONNECTIONS AND DIRECTION WITH DSHOT SET POSTIIVE

    GPIO 14 = Front right motor (M2) - Spin: Clockwise 
    GPIO 12 = Front Left Motor (M4) - Spin: CCW 
    GPIO 13 = Back Left Motor (M3) - Spin: CW 
    GPIO 15 = Back Right Motor (M1) - Spin: CCW 


    https://cookierobotics.com/066/ 
    motor mixing file for X quadcopter

    
    */





    PIO pio = pio0;
    DShotEncoder dshot(pio, 15); // gpio pin 14 for motor 1 testing

    // if init fails just loop forever
    if (!dshot.init(true))
    {
        printf("Failed to init DShot\n");
        while (1)
        {
        }
    }

    // arm drone by sending 0 throttle for 300ms+  (2 sec in this case)
    dshot.sendThrottle(0.0);
    sleep_ms(2000);

    // ramp up speed test
    int steps = 10000;
    for (int i = 0; i < steps; i++)
    {
        double t = (double)i / (double)steps; // t: 0.0 to 1.0
        dshot.sendThrottle(0.05);
        sleep_ms(1);
        printf("ramping up speed, at value of %lf",t);
    }

    // ramp down speed
    for (int i = 0; i < steps; i++)
    {
        double t = 1.0 - (double)i / (double)steps; // t: 1.0 to 0.0
        dshot.sendThrottle(0.05);
        sleep_ms(1);
        printf("ramping down speed, at value of %lf",t);
    }

    // turn off after test
    dshot.sendThrottle(0.0);
    printf("Turned off test success");

    return 0;
}