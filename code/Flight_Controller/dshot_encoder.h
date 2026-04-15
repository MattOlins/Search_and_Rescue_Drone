#ifndef DSHOT_ENCODER_H
#define DSHOT_ENCODER_H

#include "hardware/pio.h"
#include <stdint.h>

// Define DSHOT ranges
static const uint16_t MIN_THROTTLE_COMMAND = 48;
static const uint16_t MAX_THROTTLE_COMMAND = 2047;

class DShotEncoder
{
public:
    DShotEncoder(PIO p, uint dshot_gpio) : pio(p), dshot_gpio(dshot_gpio) {}

    bool init(bool enable_repeat);
    void sendCommand(uint16_t c);
    void sendThrottle(double t);
    void stop();

private:
    uint16_t getThrottleCommand(double t);

    PIO pio;
    int pio_sm = -1;
    uint dshot_gpio;
    uint pio_offset;
};

#endif