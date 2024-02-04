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
