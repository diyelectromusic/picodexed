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

// 0/undef 1=Serial 2=Message 4=SysEx
// Or use combinations of all three...
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

MidiType CMIDIDevice::ParseStatus (uint8_t ucStatus)
{
    if (ucStatus < 0x80) {
        // Not a status byte
        return InvalidType;
    }
    if (ucStatus == Undefined_F4|| ucStatus == Undefined_F5 || ucStatus == Undefined_F9 || ucStatus == Undefined_FD) {
        // Undefined status
        return InvalidType;
    }
    if (ucStatus >= 0xF0) {
        // System message
        return MidiType(ucStatus);
    }
    
    // Channel message
    return MidiType(ucStatus & 0xF0);
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

#if (MIDI_DEBUG & 1)
    if (ucByte != 0xFE) {
        // Ignore active status bytes
        // Use stderr to ignore serial port buffering
        fprintf(stderr,"%02x ", ucByte);
    }
#endif
    if (m_nPendingIdx == 0)
    {
        // Starting a new message
        m_ucPending[0] = ucByte;
        if (!m_Message.processed)
        {
            // This will result in a lost message...
        }

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

        MidiType ucPendingType = ParseStatus (m_ucPending[0]);
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
                ResetMessage(false);

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
                // m_ucPending[0] is already set to SystemExclusiveStart above
                m_nPendingExpectedLength = SYSEX_MAX_SIZE;
                m_ucRunningStatus = InvalidType;
                m_Message.sysex[0] = ucPendingType;
                break;

            case SystemExclusiveEnd:
            case InvalidType:
            default:
                // Invalid type or sequence or message
                ResetMessage(true);
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

            // Do not reset everything - preserve RunningStatus
            ResetMessage(false);

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
            // are allowed only for interleaved Real Time message or SysExEnd
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
                    
                    // Do not reset pending message - that will continue where it left off...
                    return true;

                case SystemExclusiveEnd:
                    if (m_Message.sysex[0] == SystemExclusiveStart)
                    {
                        // We're finishing up a SysEx message then
                        m_Message.sysex[m_nPendingIdx] = ucByte;
                        m_nPendingIdx++;
                        m_Message.type = SystemExclusive;
                        m_Message.d1 = m_nPendingIdx & 0xFF;
                        m_Message.d2 = (uint8_t)(m_nPendingIdx >> 8);
                        m_Message.channel = 0;
                        m_Message.length = m_nPendingIdx;
                        m_Message.valid = true;

                        ResetMessage(true);
                        return true;
                    }
                    else
                    {
                        // Error
                        ResetMessage(true);
                        return false;
                    }
                    break;

                case SystemExclusiveStart:
                default:
                    // Error
                    ResetMessage(true);
                    return false;
                    break;
            }
        }

        // Add extracted data byte to pending message
        if (m_ucPending[0] == SystemExclusiveStart)
        {
            m_Message.sysex[m_nPendingIdx] = ucByte;
        }
        else
        {
            m_ucPending[m_nPendingIdx] = ucByte;
        }

        // Now we are going to check if we have reached the end of the message
        if (m_nPendingIdx >= (m_nPendingExpectedLength - 1))
        {
            if (m_ucPending[0] == SystemExclusiveStart)
            {
                // SysEx is too big!
                ResetMessage(true);
                return false;
            }

            m_Message.type = ParseStatus(m_ucPending[0]);

            if (m_Message.type < 0xF0)
            {
                m_Message.channel = 1 + m_ucPending[0] & 0x0F;
            }
            else
            {
                m_Message.channel = 0;
            }

            // Messages of Length 1 already covered.  Length>3 means SysEx
            if (m_nPendingExpectedLength == 2)
            {
                m_Message.d1 = m_ucPending[1];
                m_Message.d2 = 0;
            }
            if (m_nPendingExpectedLength == 3)
            {
                m_Message.d1 = m_ucPending[1];
                m_Message.d2 = m_ucPending[2];
            }
            m_Message.length = m_nPendingExpectedLength;

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
                    m_ucRunningStatus = InvalidType;
                    break;
            }
            ResetMessage(false);
            m_Message.valid = true;
            m_Message.processed = false;

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

void CMIDIDevice::ResetMessage (bool bFull)
{
    if (bFull) {
        m_ucRunningStatus = 0;
    }
    m_nPendingIdx = 0;
    m_nPendingExpectedLength = 0;
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
    if (m_Message.type == SystemExclusive)
    {
        // Check for a Yamaha SysEx Message
        if ((m_Message.sysex[0] == SystemExclusive) && (m_Message.sysex[1] == SYSEX_MANID_YAMAHA))
        {
            uint8_t ucSysExDevice = m_Message.sysex[2] & 0x0F;  // Yamaha SysEx Device Number
            unsigned nSysExLen = m_Message.getSysExSize();
            if ((ucSysExDevice == MIDI_SYSEX_DEVICE_ID) && (m_Message.sysex[nSysExLen-1] == SystemExclusiveEnd))
            {
                // It is recognisably a Yamaha style, complete SysEx message for our device
                SysExMessageHandler(m_Message.sysex, m_Message.getSysExSize());
            }
        }

        // Finished with this message now
        m_Message.processed = true;
    }
    else if (m_Message.channel == m_ucChannel || m_ucChannel == MIDIOmni)
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
                
            case AfterTouchChannel:
                m_pSynth->SetAfterTouch(m_Message.d1);
                break;

            case ControlChange:
                switch (m_Message.d1)
                {
                    case 0: // BANKSEL (MSB)
                        m_pSynth->BankSelectMSB(m_Message.d2);
                        break;
                        
                    case 1: // Modulation Wheel
                        m_pSynth->SetModWheel(m_Message.d2);
                        break;
                        
                    case 2: // Breath Controller
                        m_pSynth->SetBreathControl(m_Message.d2);
                        break;
                        
                    case 4: // Foot pedal
                        m_pSynth->SetFootControl(m_Message.d2);
                        break;
                        
                    case 7: // Volume
                        m_pSynth->SetVolume(m_Message.d2);
                        break;
                        
                    case 32: // BANKSEL (LSB)
                        m_pSynth->BankSelectLSB(m_Message.d2);
                        break;

                    case 64: // Sustain
                        m_pSynth->SetSustain(m_Message.d2);
                        break;
                        
                    case 65: // Portamento
                        m_pSynth->SetPortamento(m_Message.d2);
                        break;
                        
                    case 95: // Detune
                        m_pSynth->SetMasterTune(m_Message.d2);
                        break;
                        
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
                        
                    case 126: // Poly On
                        if (m_Message.d2 == 0)
                        {
                            m_pSynth->SetMonoMode(false);
                        }
                        break;
                        
                    case 127: // Mono On
                        if (m_Message.d2 == 1)
                        {
                            m_pSynth->SetMonoMode(true);
                        }
                        break;
                        
                    default:
                        // Unsupported
                        break;
                }
                break;
        
        case ProgramChange:
            m_pSynth->ProgramChange(m_Message.d1);
            break;

        case PitchBend:
            m_pSynth->SetPitchBend(m_Message.d1, m_Message.d2);
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

void CMIDIDevice::SysExMessageHandler (const uint8_t* pMessage, const size_t nLength)
{
    int16_t sysex_return;

    sysex_return = m_pSynth->checkSystemExclusive(pMessage, nLength);
#if (MIDI_DEBUG & 4)
    fprintf(stderr, "SysEx: SYSEX handler return value: %d\n", sysex_return);
    debugSysEx(sysex_return, pMessage);
#endif

    switch (sysex_return)
    {
        case 64:
          m_pSynth->SetMonoMode(pMessage[5]);
          break;
        case 65:
          m_pSynth->setPitchbendRange(pMessage[5]);
          break;
        case 66:
          m_pSynth->setPitchbendStep(pMessage[5]);
          break;
        case 67:
          m_pSynth->setPortamentoMode(pMessage[5]);
          break;
        case 68:
          m_pSynth->setPortamentoGlissando(pMessage[5]);
          break;
        case 69:
          m_pSynth->setPortamentoTime(pMessage[5]);
          break;
        case 70:
          m_pSynth->setModWheelRange(pMessage[5]);
          break;
        case 71:
          m_pSynth->setModWheelTarget(pMessage[5]);
          break;
        case 72:
          m_pSynth->setFootControllerRange(pMessage[5]);
          break;
        case 73:
          m_pSynth->setFootControllerTarget(pMessage[5]);
          break;
        case 74:
          m_pSynth->setBreathControllerRange(pMessage[5]);
          break;
        case 75:
          m_pSynth->setBreathControllerTarget(pMessage[5]);
          break;
        case 76:
          m_pSynth->setAftertouchRange(pMessage[5]);
          break;
        case 77:
          m_pSynth->setAftertouchTarget(pMessage[5]);
          break;
        case 100:
          // load sysex-data into voice memory
          m_pSynth->loadVoiceParameters(pMessage);
          break;
        default:
          if(sysex_return >= 300 && sysex_return < 500)
          {
            m_pSynth->setVoiceDataElement(pMessage[4] + ((pMessage[3] & 0x03) * 128), pMessage[5]);
            switch(pMessage[4] + ((pMessage[3] & 0x03) * 128))
            {
              case 134:
                m_pSynth->notesOff();
                break;
            }
          }
          else if(sysex_return >= 500 && sysex_return < 600)
          {
            // SysEx send voice (sysex_return-500) request - NOT SUPPORTED
          }
          break;
    }
}

void CMIDIDevice::debugSysEx (int16_t sysex_return, const uint8_t* pMessage)
{
#if (MIDI_DEBUG & 4)
    switch (sysex_return)
    {
        case -1:
          fprintf(stderr, "SysEx: Error: SysEx end status byte not detected.\n");
          break;
        case -2:
          fprintf(stderr, "SysEx: Error: SysEx vendor not Yamaha.\n");
          break;
        case -3:
          fprintf(stderr, "SysEx: Error: Unknown SysEx parameter change.\n");
          break;
        case -4:
          fprintf(stderr, "SysEx: Error: Unknown SysEx voice or function.\n");
          break;
        case -5:
          fprintf(stderr, "SysEx: Error: Not a SysEx voice bulk upload.\n");
          break;
        case -6:
          fprintf(stderr, "SysEx: Error: Wrong length for SysEx voice bulk upload (not 155).\n");
          break;
        case -7:
          fprintf(stderr, "SysEx: Error: Checksum error for one voice.\n");
          break;
        case -8:
          fprintf(stderr, "SysEx: Error: Not a SysEx bank bulk upload.\n");
          break;
        case -9:
          fprintf(stderr, "SysEx: Error: Wrong length for SysEx bank bulk upload (not 4096).\n");
          break;
        case -10:
          fprintf(stderr, "SysEx: Error: Checksum error for bank.\n");
          break;
        case -11:
          fprintf(stderr, "SysEx: Error: Unknown SysEx message.\n");
          break;
        case 64:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 65:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 66:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 67:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 68:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 69:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 70:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 71:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 72:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 73:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 74:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 75:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 76:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 77:
          fprintf(stderr, "SysEx: SysEx Function parameter change: %d Value %d.\n",pMessage[4],pMessage[5]);
          break;
        case 100:
          fprintf(stderr, "SysEx: One Voice bulk upload.\n");
          break;
        case 200:
          fprintf(stderr, "SysEx: Bank bulk upload - NOT SUPPORTED.\n");
          break;
        default:
          if(sysex_return >= 300 && sysex_return < 500)
          {
            fprintf(stderr, "SysEx: SysEx voice parameter change: Parameter %d value: %d\n",pMessage[4] + ((pMessage[3] & 0x03) * 128), pMessage[5]);
          }
          else if(sysex_return >= 500 && sysex_return < 600)
          {
            fprintf(stderr, "SysEx: SysEx send voice %u request - NOT SUPPORTED.\n",sysex_return-500);
          }
          break;
    }
#endif
}

void CMIDIDevice::MIDIDump (void)
{
#if (MIDI_DEBUG & 2)
    if (m_Message.valid && !m_Message.processed && m_Message.type != ActiveSensing)
    {
        if (m_Message.type == SystemExclusive)
        {
            fprintf(stderr,"T:%02x L:%d 0x%02x%02x%02x%02x%02x%02x%02x%02x\n", m_Message.type, m_Message.getSysExSize(), m_Message.sysex[0], m_Message.sysex[1], m_Message.sysex[2], m_Message.sysex[3], m_Message.sysex[4], m_Message.sysex[5], m_Message.sysex[6], m_Message.sysex[7]);
        }
        else
        {
            fprintf(stderr,"T:%02x C:%02x D:%02x D:%02x\n", m_Message.type, m_Message.channel, m_Message.d1, m_Message.d2);
        }
    }
#endif
}