//
// mididevice.cpp
//

#include "mididevice.h"
#include "picodexed.h"

#define MIDI_NOTE_OFF		0x80
#define MIDI_NOTE_ON		0x90
#define MIDI_CONTROL_CHANGE	0xB0
#define MIDI_PROGRAM_CHANGE	0xC0
	#define MIDI_CC_ALL_SOUND_OFF	120
	#define MIDI_CC_ALL_NOTES_OFF	123
#define MIDI_PITCH_BEND		0xE0

#define MIDI_SYSTEM_EXCLUSIVE_BEGIN	0xF0
#define MIDI_SYSTEM_EXCLUSIVE_END	0xF7
#define MIDI_TIMING_CLOCK	0xF8
#define MIDI_ACTIVE_SENSING	0xFE

CMIDIDevice::CMIDIDevice (CPicoDexed *pSynth)
:
	m_pSynth(pSynth)
{
}

CMIDIDevice::~CMIDIDevice (void)
{
}

void CMIDIDevice::SetChannel (uint8_t ucChannel)
{
	m_ucChannel = ucChannel;
}

uint8_t CMIDIDevice::GetChannel (void)
{
	return m_ucChannel;
}

void CMIDIDevice::MIDIMessageHandler (const uint8_t *pMessage, size_t nLength)
{
	// The packet contents are just normal MIDI data - see
	// https://www.midi.org/specifications/item/table-1-summary-of-midi-message

	uint8_t ucStatus  = pMessage[0];
	uint8_t ucChannel = 1 + ucStatus & 0x0F;
	uint8_t ucType    = ucStatus >> 4;

	// Valid channels values internally = 1-16, MIDIDisabled or MIDIOmni
	if (m_ucChannel == ucChannel || m_ucChannel == MIDIOmni)
	{
		switch (ucType)
		{
		case MIDI_NOTE_ON:
			if (nLength < 3)
			{
				break;
			}

			if (pMessage[2] > 0)
			{
				if (pMessage[2] <= 127)
				{
					m_pSynth->keydown (pMessage[1], pMessage[2]);
				}
			}
			else
			{
				m_pSynth->keyup (pMessage[1]);
			}
			break;

		case MIDI_NOTE_OFF:
			if (nLength < 3)
			{
				break;
			}

			m_pSynth->keyup (pMessage[1]);
			break;

		case MIDI_CONTROL_CHANGE:
			if (nLength < 3)
			{
				break;
			}

			switch (pMessage[1])
			{
			case MIDI_CC_ALL_SOUND_OFF:
				m_pSynth->panic ();
				break;

			case MIDI_CC_ALL_NOTES_OFF:
				// As per "MIDI 1.0 Detailed Specification" v4.2
				// From "ALL NOTES OFF" states:
				// "Receivers should ignore an All Notes Off message while Omni is on (Modes 1 & 2)"
				if (m_ucChannel != MIDIOmni)
				{
					m_pSynth->notesOff ();
				}
				break;
			}
			break;
		
		case MIDI_PROGRAM_CHANGE:			
			break;

		case MIDI_PITCH_BEND:
			break;

		default:
			break;
		}
	}
}
