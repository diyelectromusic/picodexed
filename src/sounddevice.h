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
#ifndef _sounddevice_h
#define _sounddevice_h

#include "pico/audio_i2s.h"

// Example callback that just fills buffer with zeros:
//
//     void getSamples (int16_t *pBuffer, size_t BufferSize) {
//         for (int i=0; i<BufferSize; i++) {
//             pBuffer[i] = 0;
//         }
//         return;
//     }
//
typedef void (*buffer_callback)(int16_t *pBuffer, size_t BufferSize);

class CSoundDevice
{
public:
    CSoundDevice (void);
    ~CSoundDevice (void);

    bool Init (uint32_t sample_rate);
    bool Update (buffer_callback cb);

private:
    bool InitI2S (audio_format_t *pAudioFormat, audio_buffer_pool *pBufferPool);
    bool InitPWM (audio_format_t *pAudioFormat, audio_buffer_pool *pBufferPool);

private:
    struct audio_buffer_pool *pAudioPool;
};

#endif
