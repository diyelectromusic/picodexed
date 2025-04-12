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
#include "usbmidi.h"
#include "usbtask.h"

CUSBMIDIDevice::CUSBMIDIDevice (CPicoDexed *pSynth)
:    CMIDIDevice(pSynth)
{
}

CUSBMIDIDevice::~CUSBMIDIDevice (void)
{
}

bool CUSBMIDIDevice::Init (void)
{
    // init device stack on configured roothub port
    usbMidiTaskInit();
    
    fprintf(stderr, "USB MIDI Enabled\n");

    return true;
};

void CUSBMIDIDevice::Process (void)
{
    if (usbMidiTaskProcess())
    {
        m_nReadMidiData = usbMidiTaskRead(m_MidiData, USB_MIDI_SIZE);

        // If MIDI disabled, just need to return
        uint8_t ucChannel = GetChannel();
        if (ucChannel == MIDIDisabled) {
            return;
        }

        m_nRxReadPtr = 0;
        while (m_nRxReadPtr < m_nReadMidiData)
        {
            if (!MIDIParser()) {
                // If Parser fails, give up on the data read so far
            }
            else
            {
                // Process the message found
                MIDIDump();
                MIDIMessageHandler();
            }
        }
    }
}

bool CUSBMIDIDevice::Read (uint8_t *pData)
{
    if (m_nRxReadPtr >= USB_MIDI_SIZE) {
        // Run out of data!
        return false;
    }
    // Grab a byte from the buffer
    *pData = m_MidiData[m_nRxReadPtr];
    m_nRxReadPtr ++;
    return true;
}
