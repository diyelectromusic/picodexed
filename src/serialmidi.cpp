#include "serialmidi.h"

CSerialMIDIDevice::CSerialMIDIDevice (CPicoDexed *pSynth)
:	CMIDIDevice(pSynth)
{
}

CSerialMIDIDevice::~CSerialMIDIDevice (void)
{
}

bool CSerialMIDIDevice::Init (void)
{
	return true;
};

void CSerialMIDIDevice::Process (void)
{
}
