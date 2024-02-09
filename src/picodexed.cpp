#include "pico/multicore.h"
#include "picodexed.h"
#include "pico_perf.h"
#define PROGMEM 
#include "voices.h"

#define NUM_BANKS 8
#define NUM_VOICES 32
#define VOICE_SYX_SIZE 128

CPicoDexed *m_pPicoDexed = 0;

uint8_t sDefaultVoice[156] =    // Brass 1
{
             49,  99,  28,  68,  98,  98,  91,   0, 
             39,  54,  50,   1,   1,   4,   0,   2, 
             82,   0,   1,   0,   7,  77,  36,  41, 
             71,  99,  98,  98,   0,  39,   0,   0, 
              3,   3,   0,   0,   2,  98,   0,   1, 
              0,   8,  77,  36,  41,  71,  99,  98, 
             98,   0,  39,   0,   0,   3,   3,   0, 
              0,   2,  99,   0,   1,   0,   7,  77, 
             76,  82,  71,  99,  98,  98,   0,  39, 
              0,   0,   3,   3,   0,   0,   2,  99, 
              0,   1,   0,   5,  62,  51,  29,  71, 
             82,  95,  96,   0,  27,   0,   7,   3, 
              1,   0,   0,   0,  86,   0,   0,   0, 
             14,  72,  76,  99,  71,  99,  88,  96, 
              0,  39,   0,  14,   3,   3,   0,   0, 
              0,  98,   0,   0,   0,  14,  84,  95, 
             95,  60,  50,  50,  50,  50,  21,   7, 
              1,  37,   0,   5,   0,   0,   4,   3, 
             24,  66,  82,  65,  83,  83,  32,  32, 
             32,  49,  32,  63
};

CPicoDexed::CPicoDexed ()
:
    m_Dexed(DEXED_POLYPHONY,DEXED_SAMPLE_RATE),
    m_SoundOutput (),
    m_USBMIDI (this),
    m_SerialMIDI (this)
{
    m_pPicoDexed = this;
};

bool CPicoDexed::Init (void)
{
    InitControllers();
    m_nBanks = sizeof(progmem_bank) / (NUM_VOICES*VOICE_SYX_SIZE);
    if (m_nBanks != NUM_BANKS)
    {
        printf("Note: Number of banks is different to NUM_BANKS\n");
    }
    else
    {
        printf("Number of Voice Banks=%d\n",m_nBanks);
    }
    m_nCurrentBank = 0;
    m_nCurrentVoice = 0;
    ProgramChange(m_nCurrentVoice);

    m_SoundOutput.Init (DEXED_SAMPLE_RATE, I2S_DATA_PIN, I2S_BCLK_PIN);
    m_USBMIDI.Init ();
    m_SerialMIDI.Init ();
    
    m_SerialMIDI.SetChannel(MIDI_CHANNEL);
    
    // Start the multicore application - all sound processing will happen in core 1.
    multicore_launch_core1(core1_entry);
    
    return true;
}

void CPicoDexed::Process (void)
{
    //timingOn(2);
    m_USBMIDI.Process ();
    m_SerialMIDI.Process ();
    //timingOff(2);
}

void CPicoDexed::ProgramChange (uint8_t ucProgram)
{
    if (ucProgram < NUM_VOICES)
    {
        // Voice in current bank
        m_Dexed.decodeVoice(m_voice, progmem_bank[m_nCurrentBank][ucProgram]);
        m_Dexed.loadVoiceParameters(m_voice);
    }
    else if (ucProgram < (m_nBanks-m_nCurrentBank)*NUM_VOICES)
    {
        // Voice is in another bank.
        // NB: Always starts from the currently selected bank.
        int8_t ucBank = ucProgram / NUM_VOICES;
        ucProgram = ucProgram - ucBank*NUM_VOICES;
        ucBank += m_nCurrentBank;

        // Now load this voice from this bank
        m_Dexed.decodeVoice(m_voice, progmem_bank[ucBank][ucProgram]);
        m_Dexed.loadVoiceParameters(m_voice);
    }
    else
    {
        // Out of range of what can be selected
        return;
    }
    m_nCurrentVoice = ucProgram;
}

void CPicoDexed::BankSelectMSB (uint8_t ucMSB)
{
    // Only LSB is significant, so ignore MSB.
    // Yes this means that any MSB will select a bank.
}

void CPicoDexed::BankSelectLSB (uint8_t ucLSB)
{
    if (ucLSB < NUM_BANKS)
    {
        // Change bank.
        // NB: Requires a ProgramChange too to select a new voice
        m_nCurrentBank = ucLSB;
    }
}

void CPicoDexed::InitControllers (void)
{
    m_Dexed.setGain((uint8_t)127); // Full volume

    m_Dexed.setPBController(2, 0); // 2 semitone pitch bend

    // Set other controllers to full range
    m_Dexed.setMWController(99, 1, 0);
    m_Dexed.setFCController(99, 1, 0);
    m_Dexed.setBCController(99, 1, 0);
    m_Dexed.setATController(99, 1, 0);

    m_Dexed.ControllersRefresh();
}

void CPicoDexed::SetAfterTouch (uint8_t nAfterTouch)
{
    if (nAfterTouch < 128)
    {
        m_Dexed.setAftertouch(nAfterTouch);
        m_Dexed.ControllersRefresh();
    }
}

void CPicoDexed::SetModWheel (uint8_t nModWheel)
{
    if (nModWheel < 128)
    {
        m_Dexed.setModWheel (nModWheel);
        m_Dexed.ControllersRefresh();
    }
}

void CPicoDexed::SetBreathControl (uint8_t nBC)
{
    if (nBC < 128)
    {
        m_Dexed.setBreathController (nBC);
        m_Dexed.ControllersRefresh();
    }
}

void CPicoDexed::SetFootControl (uint8_t nFC)
{
    if (nFC < 128)
    {
        m_Dexed.setFootController(nFC);
    }
}

void CPicoDexed::SetVolume (uint8_t nVolume)
{
    if (nVolume < 128)
    {
        m_Dexed.setGain(nVolume);
    }
}

void CPicoDexed::SetSustain (uint8_t nSustain)
{
    if (nSustain < 64)
    {
        m_Dexed.setSustain(false);
    }
    else if (nSustain < 127)
    {
        m_Dexed.setSustain(true);
    }
}

void CPicoDexed::SetPortamento (uint8_t nPortamento)
{
    if (nPortamento < 64)
    {
        m_Dexed.setPortamento(0,0,0);
    }
    else if (nPortamento < 127)
    {
        m_Dexed.setPortamento(1,1,60);
    }
}

void CPicoDexed::SetMasterTune (uint8_t nTuning)
{
    if (nTuning < 128)
    {
        m_Dexed.setMasterTune(nTuning);
    }
}

void CPicoDexed::SetPitchBend (uint8_t nVal1, uint8_t nVal2)
{
    if (nVal1 < 128 && nVal2 < 128)
    {
        m_Dexed.setPitchbend(nVal1, nVal2);
    }
}

void CPicoDexed::SetMonoMode (bool bMono)
{
    m_Dexed.setMonoMode(bMono);
}

void CPicoDexed::SetMIDIChannel (uint8_t ucChannel)
{
    m_USBMIDI.SetChannel (ucChannel);
    m_SerialMIDI.SetChannel (ucChannel);

}

void CPicoDexed::keyup (uint8_t pitch)
{
    ledOff();
    m_Dexed.keyup ((int16_t)pitch);
}

void CPicoDexed::keydown (uint8_t pitch, uint8_t velocity)
{
    ledOn();
    m_Dexed.keydown ((int16_t)pitch, velocity);
}

void CPicoDexed::panic(void)
{
    m_Dexed.panic ();
}

void CPicoDexed::notesOff(void)
{
    m_Dexed.notesOff ();
}

void CPicoDexed::loadVoiceParameters (uint8_t* data)
{
    m_Dexed.loadVoiceParameters (data);
}

void CPicoDexed::ProcessSound (void)
{
    m_SoundOutput.Update(CPicoDexed::SampleCallback);
}

void CPicoDexed::SampleCallback (int16_t *pBuffer, size_t BufferSize)
{
    m_pPicoDexed->m_Dexed.getSamples((int16_t *)pBuffer, (uint16_t)BufferSize);
}

void CPicoDexed::core1_entry (void)
{
    while (1)
    {
        timingToggle(3);
        m_pPicoDexed->ProcessSound();
    }
}
