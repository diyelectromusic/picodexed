#include "pico/multicore.h"
#include "ssd1306.h"
#include "picodexed.h"
#include "pico_perf.h"
#include "display.h"
#include "encoder.h"
#define PROGMEM 
#include "voices.h"

#define NUM_BANKS 8
#define NUM_VOICES 32
#define VOICE_SYX_SIZE 128
#define VOICE_SIZE 156
#define VOICE_NAME_SIZE 10
#define VOICE_NAME (VOICE_SIZE-VOICE_NAME_SIZE-1)

CPicoDexed *m_pPicoDexed = 0;

uint8_t sDefaultVoice[VOICE_SIZE] =    // Brass 1
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
    m_Dexed(PICODEXED_POLYPHONY,PICODEXED_SAMPLE_RATE),
    m_SoundOutput (),
    m_USBMIDI (this),
    m_SerialMIDI (this)
{
    m_pPicoDexed = this;
};


bool CPicoDexed::Init (void)
{
    m_Display.Init();
    m_Display.Logo();
    m_Display.Update();
    sleep_ms(2000);

    m_bDisplayChanged = false;

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
    m_bBankSel = false;
    ProgramChange(m_nCurrentVoice);

    m_SoundOutput.Init (PICODEXED_SAMPLE_RATE);
    m_USBMIDI.Init ();
    m_SerialMIDI.Init ();
    
    m_SerialMIDI.SetChannel(MIDI_CHANNEL);
    m_USBMIDI.SetChannel(MIDI_CHANNEL);

    m_Encoder.Init(ENCODER_A_PIN, ENCODER_B_PIN);
    m_nRotation = 0;

    printf("Starting audio processing...");

    // Start the multicore application - all sound processing will happen in core 1.
    multicore_launch_core1(core1_entry);

    m_Display.Update();
    
    return true;
}

void CPicoDexed::Process (void)
{
    //timingOn(2);
    m_USBMIDI.Process ();
    m_SerialMIDI.Process ();

    if (m_bDisplayChanged)
    {
        m_Display.Update();
        m_bDisplayChanged = false;
    }

    int nNewRot = m_Encoder.GetRotation();
    if (nNewRot > m_nRotation) {
        // Increasing
        int nNext = m_nCurrentBank*NUM_VOICES + m_nCurrentVoice + 1;
        if (nNext >= NUM_BANKS*NUM_VOICES) nNext = 0;
        BankSelectLSB((nNext / NUM_VOICES) % NUM_BANKS);
        ProgramChange(nNext % NUM_VOICES);
    }
    else if (nNewRot < m_nRotation) {
        // Decreasing
        int nNext = m_nCurrentBank*NUM_VOICES + m_nCurrentVoice - 1;
        if (nNext < 0) nNext = NUM_BANKS*NUM_VOICES - 1;
        BankSelectLSB((nNext / NUM_VOICES) % NUM_BANKS);
        ProgramChange(nNext % NUM_VOICES);
    }
    else {
        // Ignore
    }
    m_nRotation = nNewRot;
    //timingOff(2);
}

void CPicoDexed::DisplayVoiceName()
{
    // Take voice name from current loaded voice parameters.
    char sVoiceName[VOICE_NAME_SIZE+1];
    memcpy(sVoiceName, &m_voice[VOICE_NAME], VOICE_NAME_SIZE);
    sVoiceName[VOICE_NAME_SIZE] = 0;

    char sNumbers[10];
    memset(sNumbers,0,10);
    sprintf(sNumbers, "%d:%2d", m_nCurrentBank, m_nCurrentVoice);

    m_Display.PrintDual(sNumbers, sVoiceName);
    //printf("Setting voice: %d (%d) %s\n", m_nCurrentVoice, m_nCurrentBank, sVoiceName);
    m_bDisplayChanged = true;
}

void CPicoDexed::ProgramChange (uint8_t ucProgram)
{
    if (m_bBankSel)
    {
        m_bBankSel = false;
        if (ucProgram < NUM_VOICES)
        {
            // Voice in current bank
            m_Dexed.decodeVoice(m_voice, progmem_bank[m_nCurrentBank][ucProgram]);
            m_Dexed.loadVoiceParameters(m_voice);
        }
        else
        {
            // Out of range of what can be selected
            return;
        }
    }
    else
    {
        // This is a PC on its own so map onto first 0..127 voices
        int8_t ucBank = ucProgram / NUM_VOICES;
        ucProgram = ucProgram - ucBank*NUM_VOICES;

        // Now load this voice from this bank
        m_Dexed.decodeVoice(m_voice, progmem_bank[ucBank][ucProgram]);
        m_Dexed.loadVoiceParameters(m_voice);

        m_nCurrentBank = ucBank;
    }
    m_nCurrentVoice = ucProgram;
    DisplayVoiceName();
}

void CPicoDexed::BankSelectMSB (uint8_t ucMSB)
{
    // Only LSB is significant, so ignore MSB.
    // Yes this means that any MSB will select a bank.
}

void CPicoDexed::BankSelectLSB (uint8_t ucLSB)
{
    ucLSB = ucLSB % NUM_BANKS;
    // Change bank.
    // NB: Requires a ProgramChange too to select a new voice
    m_nCurrentBank = ucLSB;
    m_bBankSel = true;
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

int16_t CPicoDexed::checkSystemExclusive (const uint8_t* pMessage, const size_t nLength)
{
    return m_Dexed.checkSystemExclusive(pMessage, nLength);
}

void CPicoDexed::setPitchbendRange(uint8_t range)
{
    m_Dexed.setPitchbendRange(constrain (range, 0, 12));
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setPitchbendStep(uint8_t step)
{
    m_Dexed.setPitchbendStep(constrain (step, 0, 12));
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setPortamentoMode(uint8_t mode)
{
    m_Dexed.setPortamentoMode(constrain (mode, 0, 1));
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setPortamentoGlissando(uint8_t glissando)
{
    m_Dexed.setPortamentoGlissando(constrain (glissando, 0, 1));
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setPortamentoTime(uint8_t time)
{
    m_Dexed.setPortamentoTime(constrain (time, 0, 99));
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setModWheelRange(uint8_t range)
{
    m_Dexed.setMWController(range, m_Dexed.getModWheelTarget(), 0);
//  m_Dexed.setModWheelRange(constrain(range, 0, 99));  replaces with the above due to wrong constrain on dexed_synth module.
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setModWheelTarget(uint8_t target)
{
    m_Dexed.setModWheelTarget(constrain(target, 0, 7));
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setFootControllerRange(uint8_t range)
{
    m_Dexed.setFCController(range, m_Dexed.getFootControllerTarget(), 0);
//  m_Dexed.setFootControllerRange(constrain(range, 0, 99));
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setFootControllerTarget(uint8_t target)
{
    m_Dexed.setFootControllerTarget(constrain(target, 0, 7));
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setBreathControllerRange(uint8_t range)
{
    m_Dexed.setBCController(range, m_Dexed.getBreathControllerTarget(), 0);
//  m_Dexed.setBreathControllerRange(constrain(range, 0, 99));
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setBreathControllerTarget(uint8_t target)
{
    m_Dexed.setBreathControllerTarget(constrain(target, 0, 7));
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setAftertouchRange(uint8_t range)
{
    m_Dexed.setATController(range, m_Dexed.getAftertouchTarget(), 0);
//  m_pTG[nTG]->setAftertouchRange(constrain(range, 0, 99));
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setAftertouchTarget(uint8_t target)
{
    m_Dexed.setAftertouchTarget(constrain(target, 0, 7));
    m_Dexed.ControllersRefresh();
}

void CPicoDexed::setVoiceDataElement(uint8_t data, uint8_t number)
{
    m_Dexed.setVoiceDataElement(constrain(data, 0, 155),constrain(number, 0, 99));
//  m_Dexed.doRefreshVoice();
}

void CPicoDexed::loadVoiceParameters (const uint8_t* data)
{
    uint8_t voice[161];

    memcpy(voice, data, sizeof(uint8_t)*161);

    // fix voice name
    for (uint8_t i = 0; i < 10; i++)
    {
        if (voice[151 + i] > 126) // filter characters
            voice[151 + i] = 32;
    }

    m_Dexed.loadVoiceParameters(&voice[6]);
    m_Dexed.doRefreshVoice();
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
//        timingToggle(3);
        m_pPicoDexed->ProcessSound();
    }
}
