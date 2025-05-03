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
#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "serialmidi.h"
#include "pico_perf.h"
#include "config.h"

// Core principles for interrupt-driven serial reception taken from
// https://github.com/raspberrypi/pico-examples/blob/master/uart/uart_advanced/uart_advanced.c

#if MIDI_UART==1
#define M_UART   uart1
#else
#define M_UART   uart0
#endif
#define M_BAUD   31250
#define M_DATA   8
#define M_STOP   1
#define M_PARITY UART_PARITY_NONE
#define M_TX_PIN MIDI_TX_PIN
#define M_RX_PIN MIDI_RX_PIN

// Helper routine to link the ISR to the class handler
CSerialMIDIDevice *pSerialMIDIDevice;
void irq_midiRxHandler (void)
{
    pSerialMIDIDevice->midiRxHandler ();
}



CSerialMIDIDevice::CSerialMIDIDevice (CPicoDexed *pSynth)
:    CMIDIDevice(pSynth)
{
    pSerialMIDIDevice = this;
}

CSerialMIDIDevice::~CSerialMIDIDevice (void)
{
}

void CSerialMIDIDevice::midiRxHandler (void)
{
    while (uart_is_readable(M_UART))
    {
        uint8_t ch = uart_getc(M_UART);
        int nNextPtr = m_nRxWritePtr + 1;
        if (nNextPtr >= RX_BUF_SIZE) {
            nNextPtr = 0;
        }
        if (nNextPtr == m_nRxReadPtr) {
            // Buffer is full, so nothing we can do apart from drop characters...
        }
        else {
            m_rxbuffer[m_nRxWritePtr] = ch;
            m_nRxWritePtr = nNextPtr;
        }
    }
}


bool CSerialMIDIDevice::Init (void)
{
    // Pretty much all of this configuration code is from the uart_advanced.c example
    uart_init(M_UART, M_BAUD);
    gpio_set_function(M_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(M_RX_PIN, GPIO_FUNC_UART);
    uart_set_hw_flow(M_UART, false, false);
    uart_set_format(M_UART, M_DATA, M_STOP, M_PARITY);
    uart_set_fifo_enabled(M_UART, false);
    
    // Now configure interrupt handling and link to the ISR
    int UART_IRQ = M_UART==uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, irq_midiRxHandler);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(M_UART, true, false); // Rx interrupt only
    
    fprintf(stderr, "Serial MIDI Enabled\n");

    return true;
};

void CSerialMIDIDevice::Process (void)
{
    // If MIDI disabled, just need to return
    uint8_t ucChannel = GetChannel();
    if (ucChannel == MIDIDisabled) {
        return;
    }
    
    if (!MIDIParser()) {
        return;
    }
    MIDIDump();
    MIDIMessageHandler();
}

bool CSerialMIDIDevice::Read (uint8_t *pData)
{
    if (m_nRxReadPtr == m_nRxWritePtr) {
        // Buffer is empty
        return false;
    }
    int nNextPtr = m_nRxReadPtr + 1;
    if (nNextPtr >= RX_BUF_SIZE) {
        nNextPtr = 0;
    }
    *pData = m_rxbuffer[m_nRxReadPtr];
    m_nRxReadPtr = nNextPtr;
    return true;    
}
