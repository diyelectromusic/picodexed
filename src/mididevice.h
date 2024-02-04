#ifndef _mididevice_h
#define _mididevice_h

#include <cstdint>
#include <cstddef>

class CPicoDexed;

class CMIDIMsg
{
public:
   CMIDIMsg (void) {}
   ~CMIDIMsg (void) {}

   void Init (void) {
       channel = type = d1 = d2 = 0;
       length = 0;
       valid = false;
       processed = false;
    }   

   uint8_t  channel;
   uint8_t  type;
   uint8_t  d1;
   uint8_t  d2;
   unsigned length;
   bool     valid;
   bool     processed;
};

enum MidiType: uint8_t
{
    InvalidType           = 0x00,    ///< For notifying errors
    NoteOff               = 0x80,    ///< Channel Message - Note Off
    NoteOn                = 0x90,    ///< Channel Message - Note On
    AfterTouchPoly        = 0xA0,    ///< Channel Message - Polyphonic AfterTouch
    ControlChange         = 0xB0,    ///< Channel Message - Control Change / Channel Mode
    ProgramChange         = 0xC0,    ///< Channel Message - Program Change
    AfterTouchChannel     = 0xD0,    ///< Channel Message - Channel (monophonic) AfterTouch
    PitchBend             = 0xE0,    ///< Channel Message - Pitch Bend
    SystemExclusive       = 0xF0,    ///< System Exclusive
    SystemExclusiveStart  = SystemExclusive,   ///< System Exclusive Start
    TimeCodeQuarterFrame  = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
    SongPosition          = 0xF2,    ///< System Common - Song Position Pointer
    SongSelect            = 0xF3,    ///< System Common - Song Select
    Undefined_F4          = 0xF4,
    Undefined_F5          = 0xF5,
    TuneRequest           = 0xF6,    ///< System Common - Tune Request
    SystemExclusiveEnd    = 0xF7,    ///< System Exclusive End
    Clock                 = 0xF8,    ///< System Real Time - Timing Clock
    Undefined_F9          = 0xF9,
    Tick                  = Undefined_F9, ///< System Real Time - Timing Tick (1 tick = 10 milliseconds)
    Start                 = 0xFA,    ///< System Real Time - Start
    Continue              = 0xFB,    ///< System Real Time - Continue
    Stop                  = 0xFC,    ///< System Real Time - Stop
    Undefined_FD          = 0xFD,
    ActiveSensing         = 0xFE,    ///< System Real Time - Active Sensing
    SystemReset           = 0xFF,    ///< System Real Time - System Reset
};

class CMIDIDevice
{
public:
    const uint8_t MIDIDisabled=0;
    const uint8_t MIDIOmni=255;

public:
    CMIDIDevice (CPicoDexed *pSynth);
    ~CMIDIDevice (void);

    void SetChannel (uint8_t ucChannel);
    uint8_t GetChannel (void);

    virtual void Send (const uint8_t *pMessage, size_t nLength) {}
    virtual bool Read (uint8_t *pData) { return false; }

protected:
    void MIDIDump (void);
    void MIDIMessageHandler (void);
    bool MIDIParser (void);

private:
    void ResetMessage (void);

private:
    CPicoDexed *m_pSynth;
    uint8_t  m_ucChannel;
    unsigned m_nPendingIdx;
    uint8_t  m_ucPending[3];
    unsigned m_nPendingExpectedLength;
    uint8_t  m_ucRunningStatus;
    CMIDIMsg m_Message;
};

#endif
