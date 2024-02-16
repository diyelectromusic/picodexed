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
    bool Read (uint8_t *pData);

private:
#define USB_MIDI_SIZE 1024
    uint8_t m_MidiData[USB_MIDI_SIZE];
    int  m_nRxReadPtr;
    unsigned m_nReadMidiData;
};

#endif
