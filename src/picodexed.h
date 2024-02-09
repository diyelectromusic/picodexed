#ifndef _picodexed_h
#define _picodexed_h

#include <cstdint>
#include <cstddef>
#include "config.h"
#include "dexedadaptor.h"
#include "sounddevice.h"
#include "usbmidi.h"
#include "serialmidi.h"

#define VOICE_SIZE 156

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


    void SetMIDIChannel (uint8_t ucChannel);

    void ProgramChange (uint8_t ucProgram);
    void BankSelectMSB (uint8_t ucMSB);
    void BankSelectLSB (uint8_t ucLSB);
    
    void SetAfterTouch (uint8_t nAfterTouch);
    void SetModWheel (uint8_t nModWheel);
    void SetBreathControl (uint8_t nBC);
    void SetFootControl (uint8_t nFC);
    void SetVolume (uint8_t nVolume);
    void SetSustain (uint8_t nSustain);

private:
    void ProcessSound (void);
    static void SampleCallback (int16_t *pBuffer, size_t BufferSize);
    static void core1_entry (void);

private:
    CDexedAdapter        m_Dexed;
    CSoundDevice        m_SoundOutput;
    CUSBMIDIDevice        m_USBMIDI;
    CSerialMIDIDevice    m_SerialMIDI;

    uint8_t m_voice[VOICE_SIZE];
    unsigned m_nBanks;
    unsigned m_nCurrentVoice;
    unsigned m_nCurrentBank;
    unsigned m_nVolume;
};

#endif
