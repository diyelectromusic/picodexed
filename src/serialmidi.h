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
    bool Read (uint8_t *pData);

    void midiRxHandler (void);
    
private:
#define RX_BUF_SIZE 128
    uint8_t m_rxbuffer[RX_BUF_SIZE];
    int m_nRxWritePtr;
    int m_nRxReadPtr;
};

#endif
