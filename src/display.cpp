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

void CDisplay::Update (void)
{
    ssd1306_show(&disp);
}
