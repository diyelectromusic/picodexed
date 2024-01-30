#include "picodexed.h"
#include "pico_perf.h"

CPicoDexed *m_pPicoDexed = 0;

CPicoDexed::CPicoDexed ()
:
	m_Dexed(DEXED_POLYPHONY,DEXED_SAMPLE_RATE),
	m_SoundOutput (),
	m_USBMIDI (this),
	m_SerialMIDI (this)
{
	m_pPicoDexed = this;
};

bool CPicoDexed::Init (void)
{
	m_SoundOutput.Init (DEXED_SAMPLE_RATE, I2S_DATA_PIN, I2S_BCLK_PIN);
	m_USBMIDI.Init ();
	m_SerialMIDI.Init ();
	
	m_SerialMIDI.SetChannel(1);
	
	return true;
}

void CPicoDexed::Process (void)
{
    timingToggle(2);
	m_USBMIDI.Process ();
	m_SerialMIDI.Process ();
	
	ProcessSound();
}

void CPicoDexed::ProgramChange (uint8_t ucProgram)
{
}

void CPicoDexed::SetMIDIChannel (uint8_t ucChannel)
{
	m_USBMIDI.SetChannel (ucChannel);
	m_SerialMIDI.SetChannel (ucChannel);

}

void CPicoDexed::keyup (uint8_t pitch)
{
	ledOff();
	timingOff(3);
	m_Dexed.keyup ((int16_t)pitch);
}

void CPicoDexed::keydown (uint8_t pitch, uint8_t velocity)
{
	ledOn();
	timingOn(3);
	m_Dexed.keydown ((int16_t)pitch, velocity);
}

void CPicoDexed::panic(void)
{
	m_Dexed.panic ();
}

void CPicoDexed::notesOff(void)
{
	m_Dexed.notesOff ();
}

void CPicoDexed::loadVoiceParameters (uint8_t* data)
{
	m_Dexed.loadVoiceParameters (data);
}

void CPicoDexed::ProcessSound (void)
{
	m_SoundOutput.Update(CPicoDexed::SampleCallback);
}

void CPicoDexed::SampleCallback (int16_t *pBuffer, size_t BufferSize)
{
	m_pPicoDexed->m_Dexed.getSamples((int16_t *)pBuffer, (uint16_t)BufferSize);
}
