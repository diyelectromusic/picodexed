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
#include "usbtask.h"

void usbMidiTaskInit (void)
{
    // init device stack on configured roothub port
    tud_init(BOARD_TUD_RHPORT);

    return;
};

uint32_t usbMidiTaskProcess (void)
{
    // tinyusb device task
    tud_task();
    return tud_midi_available();
}

uint32_t usbMidiTaskRead (uint8_t *pDataStream, unsigned nStreamSize)
{
    // MIDI processing
    if (nStreamSize >= MIDI_USB_PKT_SIZE)
    {
        if ( tud_midi_available() )
        {
            // 2 Options here:
            //  - Read a single 4-byte MIDI packet
            //  - Read a stream of packets
            //
            // Advantage of using a stream: it handles
            // gluing together of multiple 4-byte packets
            // that form a SysEx message...
            //
            // Disadvantage of using a stream: it is possible
            // to get multiple messages back.
            //
            // However, if use the packet interface, then
            // need to know how to parse MIDI 4-byte USB packets
            // and do things like join SysEx messages ourselves.
            //
            return tud_midi_stream_read(pDataStream, nStreamSize);
//            tud_midi_packet_read(pPacket);
        }
    }
    
    return 0;
}

// ---------------------------------------------------
//
// Callbacks taken from the TinyUSB MIDI Example

// Invoked when device is mounted
void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

