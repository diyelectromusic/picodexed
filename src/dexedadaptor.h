#ifndef _dexedadapter_h
#define _dexedadapter_h

#include "synth_dexed.h"
#include <stdint.h>

class CDexedAdapter : public Dexed
{
public:
	CDexedAdapter (uint8_t maxnotes, int rate)
	: Dexed (maxnotes, rate)
	{
	}

	void loadVoiceParameters (uint8_t* data)
	{
		Dexed::loadVoiceParameters (data);
	}

	void keyup (int16_t pitch)
	{
		Dexed::keyup (pitch);
	}

	void keydown (int16_t pitch, uint8_t velo)
	{
		Dexed::keydown (pitch, velo);
	}

	void getSamples (float32_t* buffer, uint16_t n_samples)
	{
		Dexed::getSamples (buffer, n_samples);
	}

	void getSamples (int16_t* buffer, uint16_t n_samples)
	{
		Dexed::getSamples (buffer, n_samples);
	}

	void ControllersRefresh (void)
	{
		Dexed::ControllersRefresh ();
	}

	void setSustain (bool sustain)
	{
		Dexed::setSustain (sustain);
	}
};
#endif
