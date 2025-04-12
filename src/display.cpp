/*
// Simple DIY Electronic Music Projects - picoDexed
//    diyelectromusic.wordpress.com
//
// Some aspects of this code are based on the MiniDexed and Synth_Dexed projects.
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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "display.h"
#include "config.h"

extern "C" {
#include "ssd1306.h"
#include "font.h"
}

static ssd1306_t disp;

CDisplay::CDisplay (void)
{
}

CDisplay::~CDisplay (void)
{
}

bool CDisplay::Init (void)
{
    i2c_init(i2c1, 400000);
    gpio_set_function(DISPLAY_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(DISPLAY_I2C_SDA);
    gpio_pull_up(DISPLAY_I2C_SCL);
    
    disp.external_vcc=false;
    ssd1306_init(&disp, DISPLAY_W, DISPLAY_H, DISPLAY_I2C_ADDR, i2c1);
    ssd1306_clear(&disp);
    ssd1306_show(&disp);
    
    return true;
}

void CDisplay::Logo (void)
{
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 8, 12, 2, "picoDexed");
}

void CDisplay::Print (std::string sString)
{
    if (!sString.empty())
    {
        ssd1306_clear(&disp);
        ssd1306_draw_string(&disp, 0, 12, 2, sString.c_str());
    }
}

void CDisplay::PrintDual (std::string sString1, std::string sString2)
{
    if (!sString1.empty() && !sString2.empty())
    {
        ssd1306_clear(&disp);
        ssd1306_draw_string(&disp, 0, 0, 2, sString1.c_str());
        ssd1306_draw_string(&disp, 0, 16, 2, sString2.c_str());
    }
}

void CDisplay::Update (void)
{
    ssd1306_show(&disp);
}
