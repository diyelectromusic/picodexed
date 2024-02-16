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
