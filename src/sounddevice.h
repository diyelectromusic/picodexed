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
