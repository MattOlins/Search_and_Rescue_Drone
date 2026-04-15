#include "bar180.h"
#include <math.h>

static void bmp180_read_bytes(i2c_inst_t *i2c, uint8_t reg, uint8_t *buf, size_t len) {
    i2c_write_blocking(i2c, BMP180_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c, BMP180_ADDR, buf, len, false);
}

static uint16_t read_u16(i2c_inst_t *i2c, uint8_t reg) {
    uint8_t data[2];
    bmp180_read_bytes(i2c, reg, data, 2);
    return (data[0] << 8) | data[1];
}

static int16_t read_s16(i2c_inst_t *i2c, uint8_t reg) {
    return (int16_t)read_u16(i2c, reg);
}

bool bmp180_init(bmp180_t *dev, i2c_inst_t *i2c) {
    uint8_t id;
    bmp180_read_bytes(i2c, 0xD0, &id, 1);
    if (id != 0x55)
        return false;

    dev->AC1 = read_s16(i2c, 0xAA);
    dev->AC2 = read_s16(i2c, 0xAC);
    dev->AC3 = read_s16(i2c, 0xAE);
    dev->AC4 = read_u16(i2c, 0xB0);
    dev->AC5 = read_u16(i2c, 0xB2);
    dev->AC6 = read_u16(i2c, 0xB4);
    dev->B1  = read_s16(i2c, 0xB6);
    dev->B2  = read_s16(i2c, 0xB8);
    dev->MB  = read_s16(i2c, 0xBA);
    dev->MC  = read_s16(i2c, 0xBC);
    dev->MD  = read_s16(i2c, 0xBE);

    dev->oversample = 3;
    dev->sealevel_pressure = 1013.25;

    return true;
}

float bmp180_read_temperature(bmp180_t *dev) {
    uint8_t cmd = 0x2E;
    i2c_write_blocking(i2c0, BMP180_ADDR, (uint8_t[]){0xF4, cmd}, 2, false);
    sleep_ms(5);

    uint16_t UT = read_u16(i2c0, 0xF6);

    int32_t X1 = ((UT - dev->AC6) * dev->AC5) >> 15;
    int32_t X2 = ((int32_t)dev->MC << 11) / (X1 + dev->MD);
    dev->B5 = X1 + X2;

    return ((dev->B5 + 8) >> 4) / 10.0f;
}

float bmp180_read_pressure_hPa(bmp180_t *dev) {
    bmp180_read_temperature(dev);

    uint8_t cmd = 0x34 | (dev->oversample << 6);
    i2c_write_blocking(i2c0, BMP180_ADDR, (uint8_t[]){0xF4, cmd}, 2, false);

    static const int delays[4] = {5, 8, 14, 26};
    sleep_ms(delays[dev->oversample]);

    uint8_t buf[3];
    bmp180_read_bytes(i2c0, 0xF6, buf, 3);
    int32_t UP = (((int32_t)buf[0] << 16) | ((int32_t)buf[1] << 8) | buf[2]) >> (8 - dev->oversample);

    int32_t B6 = dev->B5 - 4000;
    int32_t X1 = (dev->B2 * ((B6 * B6) >> 12)) >> 11;
    int32_t X2 = (dev->AC2 * B6) >> 11;
    int32_t X3 = X1 + X2;
    int32_t B3 = (((((int32_t)dev->AC1 * 4 + X3) << dev->oversample)) + 2) >> 2;

    X1 = (dev->AC3 * B6) >> 13;
    X2 = (dev->B1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    uint32_t B4 = (dev->AC4 * (uint32_t)(X3 + 32768)) >> 15;

    uint32_t B7 = (UP - B3) * (50000 >> dev->oversample);

    int32_t p = (B7 < 0x80000000)
                ? (B7 * 2) / B4
                : (B7 / B4) * 2;

    X1 = (p >> 8) * (p >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * p) >> 16;

    p += (X1 + X2 + 3791) >> 4;

    return p / 100.0f;
}

float bmp180_read_altitude(bmp180_t *dev) {
    float pressure = bmp180_read_pressure_hPa(dev);
    return 44330.0f * (1.0f - powf(pressure / dev->sealevel_pressure, 0.1903f));
}
