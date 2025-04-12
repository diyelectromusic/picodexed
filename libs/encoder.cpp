/*
// Simple DIY Electronic Music Projects - picoDexed
//    diyelectromusic.wordpress.com
//
// Some aspects of this code are based on the Pico Rotary Encoder library by GitJer.
// See CREDITS listed here for details: https://github.com/diyelectromusic/picodexed

      MIT License
      
      Copyright (c) 2025 diyelectromusic (Kevin)
      
      Permission is hereby granted, free of charge, to any person obtaining a copy of
      this software and associated documentation files (the "Software"), to deal in
      the Software without restriction, including without limitation the rights to
      use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
      the Software, and to permit persons to whom the Software is furnished to do so,
      subject to the following conditions:
      
      The above copyright notice and this permission notice shall be included in all
      copies or substantial portions of the Software.
      
      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
      IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
      FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
      COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHERIN
      AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
      WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "encoder.h"

int CRotaryEncoder::rotation=0;

CRotaryEncoder::CRotaryEncoder(void)
{
}

bool CRotaryEncoder::Init(uint rotary_encoder_A, uint rotary_encoder_B_dummy)
{
    PIO pio = pio1;
    sm = pio_claim_unused_sm(pio, true);
    assert(sm != -1); // No free state machine...

    // B pin HAS to be consecutive to the A pin
    uint8_t rotary_encoder_B = rotary_encoder_A + 1;
    assert (rotary_encoder_B == rotary_encoder_B_dummy);
    
    // configure the used pins as input with pull up
    pio_gpio_init(pio, rotary_encoder_A);
    gpio_set_pulls(rotary_encoder_A, true, false);
    pio_gpio_init(pio, rotary_encoder_B);
    gpio_set_pulls(rotary_encoder_B, true, false);

    // load the pio program into the pio memory
    uint offset = pio_add_program(pio, &pio_rotary_encoder_program);
    // make a sm config
    pio_sm_config c = pio_rotary_encoder_program_get_default_config(offset);
    // set the 'in' pins
    sm_config_set_in_pins(&c, rotary_encoder_A);
    // set shift to left: bits shifted by 'in' enter at the least
    // significant bit (LSB), no autopush
    sm_config_set_in_shift(&c, false, false, 0);
    // set the IRQ handler
    irq_set_exclusive_handler(PIO1_IRQ_0, pio_irq_handler);
    // enable the IRQ
    irq_set_enabled(PIO1_IRQ_0, true);
    pio1_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
    // init the sm.
    // Note: the program starts after the jump table -> initial_pc = 16
    pio_sm_init(pio, sm, 16, &c);
    // enable the sm
    pio_sm_set_enabled(pio, sm, true);
    
    return true;
}

void CRotaryEncoder::SetRotation(int _rotation)
{
    rotation = _rotation;
}

int CRotaryEncoder::GetRotation(void)
{
    // Seem to get double interrupts per click...
    return rotation / 2;
}

void CRotaryEncoder::pio_irq_handler()
{
    // test if irq 0 was raised
    if (pio1_hw->irq & 1)
    {
        rotation = rotation - 1;
    }
    // test if irq 1 was raised
    if (pio1_hw->irq & 2)
    {
        rotation = rotation + 1;
    }
    // clear both interrupts
    pio1_hw->irq = 3;
}
