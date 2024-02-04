#ifndef _picodexed_h
#define _picodexed_h

#include <cstdint>
#include <cstddef>
#include "config.h"
#include "dexedadaptor.h"
#include "sounddevice.h"
#include "usbmidi.h"
#include "serialmidi.h"

class CPicoDexed
{
public:
	CPicoDexed (void);
	~CPicoDexed (void);

	bool Init (void);
	void Process (void);

	void keyup (uint8_t pitch);
	void keydown (uint8_t pitch, uint8_t velocity);

	void panic (void);
	void notesOff (void);

	void loadVoiceParameters (uint8_t* data);


	void ProgramChange (uint8_t ucProgram);
	void SetMIDIChannel (uint8_t ucChannel);

private:
	void ProcessSound (void);
	static void SampleCallback (int16_t *pBuffer, size_t BufferSize);
	static void core1_entry (void);

private:
	CDexedAdapter		m_Dexed;
	CSoundDevice		m_SoundOutput;
	CUSBMIDIDevice		m_USBMIDI;
	CSerialMIDIDevice	m_SerialMIDI;
};

#endif
