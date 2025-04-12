/*
// Simple DIY Electronic Music Projects - picoDexed
//    diyelectromusic.wordpress.com
//
// Some aspects of this code are based on the MiniDexed and Synth_Dexed projects.
// See CREDITS listed here for details: https://github.com/diyelectromusic/picodexed

      MIT License
      
      Copyright (c) 2025 diyelectromusic (Kevin)
      
      Permission is hereby granted, free of charge, to any person obtaining a copy of
      this software and associated documentation files (the "Software"), to deal in
      the Software without restriction, including without limitation the rights to
      use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
      the Software, and to permit persons to whom the Software is furnished to do so,
      subject to the following conditions:
      
      The above copyright notice and this permission notice shall be included in all
      copies or substantial portions of the Software.
      
      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
      IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
      FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
      COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHERIN
      AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
      WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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
