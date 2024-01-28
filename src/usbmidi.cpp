#include "usbmidi.h"

CUSBMIDIDevice::CUSBMIDIDevice (CPicoDexed *pSynth)
:	CMIDIDevice(pSynth)
{
}

CUSBMIDIDevice::~CUSBMIDIDevice (void)
{
}

bool CUSBMIDIDevice::Init (void)
{
	return true;
};

void CUSBMIDIDevice::Process (void)
{
}
