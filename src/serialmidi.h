#ifndef _serialmidi_h
#define _serialmidi_h

#include "mididevice.h"

class CSerialMIDIDevice : public CMIDIDevice
{
public:
	CSerialMIDIDevice (CPicoDexed *pSynth);
	~CSerialMIDIDevice (void);

	bool Init (void);
	void Process (void);

};

#endif
