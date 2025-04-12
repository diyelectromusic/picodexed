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
#ifndef _dexedadapter_h
#define _dexedadapter_h

#include "synth_dexed.h"
#include <stdint.h>
#include "pico/sync.h"

class CDexedAdapter : public Dexed
{
public:
    CDexedAdapter (uint8_t maxnotes, int rate)
    : Dexed (maxnotes, rate)
    {
        // Use one of the RPi Pico's spinlocks to protect access to Dexed during multicore execution
        spinlock_num = spin_lock_claim_unused(true);
        spinlock = spin_lock_init(spinlock_num);
    }

    void loadVoiceParameters (uint8_t* data)
    {
        spin_lock_unsafe_blocking(spinlock);
        Dexed::loadVoiceParameters (data);
        spin_unlock_unsafe(spinlock);
    }

    void keyup (int16_t pitch)
    {
        spin_lock_unsafe_blocking(spinlock);
        Dexed::keyup (pitch);
        spin_unlock_unsafe(spinlock);
    }

    void keydown (int16_t pitch, uint8_t velo)
    {
        spin_lock_unsafe_blocking(spinlock);
        Dexed::keydown (pitch, velo);
        spin_unlock_unsafe(spinlock);
    }

    void getSamples (float32_t* buffer, uint16_t n_samples)
    {
        spin_lock_unsafe_blocking(spinlock);
        Dexed::getSamples (buffer, n_samples);
        spin_unlock_unsafe(spinlock);
    }

    void getSamples (int16_t* buffer, uint16_t n_samples)
    {
        spin_lock_unsafe_blocking(spinlock);
        Dexed::getSamples (buffer, n_samples);
        spin_unlock_unsafe(spinlock);
    }

    void ControllersRefresh (void)
    {
        spin_lock_unsafe_blocking(spinlock);
        Dexed::ControllersRefresh ();
        spin_unlock_unsafe(spinlock);
    }

    void setSustain (bool sustain)
    {
        spin_lock_unsafe_blocking(spinlock);
        Dexed::setSustain (sustain);
        spin_unlock_unsafe(spinlock);
    }
    
private:
    int spinlock_num;
    spin_lock_t *spinlock;
};
#endif
