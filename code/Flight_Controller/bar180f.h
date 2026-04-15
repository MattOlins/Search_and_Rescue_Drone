#ifndef BMP180F_H
#define BMP180F_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// BMP180 / BMP085 I2C address
#define BMP180_ADDR 0x77

typedef struct {
    int16_t AC1, AC2, AC3;
    uint16_t AC4, AC5, AC6;
    int16_t B1, B2, MB, MC, MD;

    uint8_t oversample;
    float sealevel_pressure;
    int32_t B5;

} bmp180_t;
#ifdef __cplusplus
extern "C" {
#endif

// declarations



bool bmp180_init(bmp180_t *dev, i2c_inst_t *i2c);
float bmp180_read_temperature(bmp180_t *dev);
float bmp180_read_pressure_hPa(bmp180_t *dev);
float bmp180_read_altitude(bmp180_t *dev);

#ifdef __cplusplus
}
#endif


#endif