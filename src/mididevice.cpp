//
// mididevice.cpp
//
// Borrows heavily from the MIDI Parser from MiniDexed:
//   https://github.com/probonopd/MiniDexed
// and the FortySevenEffects Arduino MIDI Library:
//   https://github.com/FortySevenEffects/arduino_midi_library
//

#include "mididevice.h"
#include "picodexed.h"

// 0/undef 1=Serial 2=Message
//#define MIDI_DEBUG 2

CMIDIDevice::CMIDIDevice (CPicoDexed *pSynth)
:
	m_pSynth(pSynth)
{
    m_ucChannel = MIDIDisabled;
    m_ucRunningStatus = 0;

    m_nPendingIdx = 0;
    m_nPendingExpectedLength = 0;

    m_Message.Init();
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

bool CMIDIDevice::MIDIParser (void)
{
	// Algorithm:
	//   1. Grab a byte from the serial port.
	//   2. If not part way through a message, start a new one.
	//   3. Process channel and status/type.
	//   4. Keep processing bytes until a message is complete
	uint8_t ucByte;
	if (!Read(&ucByte)) {
		// No data to process
		return false;
	}

#if MIDI_DEBUG==1
	if (ucByte != 0xFE) {
		// Ignore active status bytes
		printf("%02x ", ucByte);
	}
#endif
	if (m_nPendingIdx == 0)
	{
		// Starting a new message
		m_ucPending[0] = ucByte;
		m_Message.Init();
		
		// Check Running Status:
		//   If a channel message i.e. between 0x80 and 0xEF inclusive
		//   and next byte is not a status byte i.e. < 0x80
		if ((m_ucRunningStatus >= 0x80) && (m_ucRunningStatus < 0xF0) && (ucByte < 0x80))
		{
			// Pre-pend running status to data
			m_ucPending[0] = m_ucRunningStatus;
			m_ucPending[1] = ucByte;
			m_nPendingIdx = 1;
		}
		
		MidiType ucPendingType = MidiType (m_ucPending[0] & 0xF0);
		switch (ucPendingType)
		{
            // 1 byte messages
            case Start:
            case Continue:
            case Stop:
            case Clock:
            case Tick:
            case ActiveSensing:
            case SystemReset:
            case TuneRequest:
                // Handle the message type directly here.
                m_Message.type    = ucPendingType;
                m_Message.channel = 0;
                m_Message.d1      = 0;
                m_Message.d2      = 0;
                m_Message.valid   = true;
				m_Message.processed = false;

                // Do not reset everything - running Status must remain unchanged.
                m_nPendingIdx = 0;
                m_nPendingExpectedLength = 0;

                return true;
                break;

            // 2 bytes messages
            case ProgramChange:
            case AfterTouchChannel:
            case TimeCodeQuarterFrame:
            case SongSelect:
                m_nPendingExpectedLength = 2;
                break;

            // 3 bytes messages
            case NoteOn:
            case NoteOff:
            case ControlChange:
            case PitchBend:
            case AfterTouchPoly:
            case SongPosition:
                m_nPendingExpectedLength = 3;
                break;

            case SystemExclusiveStart:
            case SystemExclusiveEnd:
				// not supported yet
				return false;
                break;

            default:
				// Invalid type
                ResetMessage();
                return false;
                break;
        }

		// Check if two-byte message now complete
        if ((m_nPendingIdx == 1) && (m_nPendingExpectedLength == 2))
        {
            // Reception complete
            m_Message.type    = ucPendingType;
            m_Message.channel = 1 + m_ucPending[0] & 0xF0;
            m_Message.d1      = m_ucPending[1];
            m_Message.d2      = 0; // Completed new message has 1 data byte
            m_Message.length  = 1;

            m_nPendingIdx = 0;
            m_nPendingExpectedLength = 0;
            m_Message.valid = true;
			m_Message.processed = false;

            return true;
        }
		
		// Otherwise waiting for more data
        m_nPendingIdx++;
		return MIDIParser();
	}
	else // m_nPendingIdx != 0
	{
		// Continuing a message
        // First, test if this is a status byte
        if (ucByte >= 0x80)
        {
            // Reception of status bytes in the middle of an uncompleted message
            // are allowed only for interleaved Real Time message or SysEx
            switch (ucByte)
            {
                case Clock:
                case Start:
                case Tick:
                case Continue:
                case Stop:
                case ActiveSensing:
                case SystemReset:
                    m_Message.type    = ucByte;
                    m_Message.d1      = 0;
                    m_Message.d2      = 0;
                    m_Message.channel = 0;
                    m_Message.length  = 1;
                    m_Message.valid   = true;
					m_Message.processed = false;
                    return true;

                    // System Exclusive
                case SystemExclusiveStart:
                case SystemExclusiveEnd:
					// Ignore for now...
					return true;

                default:
                    break;
            }
        }

        // Add extracted data byte to pending message
        if ((m_ucPending[0] == SystemExclusiveStart) || (m_ucPending[0] == SystemExclusiveEnd))
		{
            // Ignoring SysEx
		}
        else
		{
            m_ucPending[m_nPendingIdx] = ucByte;
		}

        // Now we are going to check if we have reached the end of the message
        if (m_nPendingIdx >= (m_nPendingExpectedLength - 1))
        {
            if ((m_ucPending[0] == SystemExclusiveStart)
            ||  (m_ucPending[0] == SystemExclusiveEnd))
            {
				// Ignoring SysEx
                return false;
            }

            m_Message.type = m_ucPending[0] & 0xF0;

            if (m_Message.type < 0xF0)
			{
                m_Message.channel = 1 + m_ucPending[0] & 0x0F;
			}
            else
			{
                m_Message.channel = 0;
			}

            m_Message.d1 = m_ucPending[1];
			if (m_nPendingExpectedLength == 3)
			{
	            m_Message.d2 = m_ucPending[2];
			}
			else
			{
	            m_Message.d2 = 0;
			}
            m_Message.length = m_nPendingExpectedLength;

            m_nPendingIdx = 0;
            m_nPendingExpectedLength = 0;

            m_Message.valid = true;
			m_Message.processed = false;

            // Activate running status (if enabled for the received type)
            switch (m_Message.type)
            {
                case NoteOff:
                case NoteOn:
                case AfterTouchPoly:
                case ControlChange:
                case ProgramChange:
                case AfterTouchChannel:
                case PitchBend:
                    // Running status enabled: store it from received message
                    m_ucRunningStatus = m_ucPending[0];
                    break;

                default:
                    // No running status
                    m_ucRunningStatus = 0;
                    break;
            }
            return true;
        }
        else
        {
            // Not reached the end yet...
            m_nPendingIdx++;

            return MIDIParser();
        }
    }
}

void CMIDIDevice::ResetMessage (void)
{
	m_ucRunningStatus = 0;
	m_nPendingIdx = 0;
	m_nPendingExpectedLength = 0;
	m_Message.Init();
}


void CMIDIDevice::MIDIMessageHandler (void)
{
	if (!m_Message.valid) {
		// No valid message to parse yet
		return;
	}

	if (m_Message.processed) {
		// Have already done this one
		return;
	}

	// Valid channels values internally = 1-16, MIDIDisabled or MIDIOmni
	if (m_Message.channel == m_ucChannel || m_ucChannel == MIDIOmni)
	{
		switch (m_Message.type)
		{
			case NoteOn:
				if (m_Message.d2 == 0)
				{
					// Process as NoteOff
					m_pSynth->keyup (m_Message.d1);
				}
				else
				{
					// NoteOn
					m_pSynth->keydown (m_Message.d1, m_Message.d2);
				}
				break;

			case NoteOff:
				m_pSynth->keyup (m_Message.d1);
				break;

			case ControlChange:
				switch (m_Message.d1)
				{
					case 120: // All Sounds Off
						m_pSynth->panic ();
						break;

					case 123: // All Notes Off
						// As per "MIDI 1.0 Detailed Specification" v4.2
						// From "ALL NOTES OFF" states:
						// "Receivers should ignore an All Notes Off message while Omni is on (Modes 1 & 2)"
						if (m_ucChannel != MIDIOmni)
						{
							m_pSynth->notesOff ();
						}
						break;
						
					default:
						// Unsupported
						break;
				}
				break;
		
		case ProgramChange:			
			break;

		case PitchBend:
			break;

		default:
			break;
		}
		
		// Finished with this message now
		m_Message.processed = true;
	}
	else
	{
		// m_ucChannel == MIDIDisabled
	}
}

void CMIDIDevice::MIDIDump (void)
{
#if MIDI_DEBUG==2
	if (m_Message.valid && !m_Message.processed)
	{
		printf("T:%02x C:%02x D:%02x D:%02x\n", m_Message.type, m_Message.channel, m_Message.d1, m_Message.d2);
	}
#endif
}