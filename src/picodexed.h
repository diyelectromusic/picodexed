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

    void SetMIDIChannel (uint8_t ucChannel);

    void ProgramChange (uint8_t ucProgram);
    void BankSelectMSB (uint8_t ucMSB);
    void BankSelectLSB (uint8_t ucLSB);
    void InitControllers (void);

    void SetAfterTouch (uint8_t nAfterTouch);
    void SetModWheel (uint8_t nModWheel);
    void SetBreathControl (uint8_t nBC);
    void SetFootControl (uint8_t nFC);
    void SetVolume (uint8_t nVolume);
    void SetSustain (uint8_t nSustain);
    void SetPortamento (uint8_t nPortamento);
    void SetMasterTune (uint8_t nTuning);
    void SetPitchBend (uint8_t nVal1, uint8_t nVal2);
    void SetMonoMode (bool bMono);

    // SysEx handling functions
    int16_t checkSystemExclusive (const uint8_t* pMessage, const size_t nLength);
    void setPitchbendRange(uint8_t range);
    void setPitchbendStep(uint8_t step);
    void setPortamentoMode(uint8_t mode);
    void setPortamentoGlissando(uint8_t glissando);
    void setPortamentoTime(uint8_t time);
    void setModWheelRange(uint8_t range);
    void setModWheelTarget(uint8_t target);
    void setFootControllerRange(uint8_t range);
    void setFootControllerTarget(uint8_t target);
    void setBreathControllerRange(uint8_t range);
    void setBreathControllerTarget(uint8_t target);
    void setAftertouchRange(uint8_t range);
    void setAftertouchTarget(uint8_t target);
    void loadVoiceParameters(const uint8_t* data);
    void setVoiceDataElement(uint8_t data, uint8_t number);

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
