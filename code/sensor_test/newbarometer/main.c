#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "bar180.h"

int main() {
    stdio_init_all();
    i2c_init(i2c0, 10000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);

    bmp180_t bmp;
    
    if (!bmp180_init(&bmp, i2c0)) {
        printf("BMP180 init failed!\n");
        return 1;
    }

    bmp.oversample = 2;
    bmp.sealevel_pressure = 1013.25;

    while (1) {
        float tempC = bmp180_read_temperature(&bmp);
        float tempF = tempC * 9.0f / 5.0f + 32.0f;
        float pressure = bmp180_read_pressure_hPa(&bmp);
        float altitude = bmp180_read_altitude(&bmp);

        printf("%.2f °C  %.2f °F  %.2f hPa  %.2f m\n",
               tempC, tempF, pressure, altitude);

        sleep_ms(250);
    }

    return 0;
}

