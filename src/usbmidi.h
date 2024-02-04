#ifndef _usbmidi_h
#define _usbmidi_h

#include "mididevice.h"

class CUSBMIDIDevice : public CMIDIDevice
{
public:
    CUSBMIDIDevice (CPicoDexed *pSynth);
    ~CUSBMIDIDevice (void);

    bool Init (void);
    void Process (void);

};

#endif
