#ifndef _encoder_h
#define _encoder_h
// Much of this code is based on code from
// https://github.com/GitJer/Some_RPI-Pico_stuff/tree/main/Rotary_encoder
//

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"

#include "pio_rotary_encoder.pio.h"

class CRotaryEncoder
{
public:
    CRotaryEncoder(void);
    ~CRotaryEncoder(void);

    // rotary_encoder_A is the pin for the A of the rotary encoder.
    // The B of the rotary encoder has to be connected to the next GPIO.
    bool Init(uint rotary_encoder_A, uint rotary_encoder_B_dummy);

    // set the current rotation to a specific value
    void SetRotation(int _rotation);

    // get the current rotation
    int GetRotation(void);

private:
    static void pio_irq_handler();

    // the pio instance
    PIO pio;
    // the state machine
    uint sm;
    // the current location of rotation
    static int rotation;
};

#endif
