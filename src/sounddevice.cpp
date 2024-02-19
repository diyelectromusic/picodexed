#include "pico/audio_i2s.h"
#include "pico/audio_pwm.h"
#include "sounddevice.h"
#include "config.h"

#define SAMPLES_PER_BUFFER 256

CSoundDevice::CSoundDevice (void)
:
    pAudioPool(nullptr)
{
}

CSoundDevice::~CSoundDevice (void)
{
}

bool CSoundDevice::Init (uint32_t sample_rate)
{
    // Setting up these structures is essentially "boiler plate" code from the
    // Raspberry Pi Pico example code...
    static audio_format_t audio_format = {
        .sample_freq = sample_rate,
        .format = AUDIO_BUFFER_FORMAT_PCM_S16,
        .channel_count = 1,
    };

    static struct audio_buffer_format producer_format = {
        .format = &audio_format,
        .sample_stride = 2
    };

    struct audio_buffer_pool *producer_pool = audio_new_producer_pool(
        &producer_format,
        3,
        SAMPLES_PER_BUFFER
    );
#if AUDIO_OUTPUT==AUDIO_I2S
    if (!InitI2S (&audio_format, producer_pool))
    {
        return false;
    }
#elif AUDIO_OUTPUT==AUDIO_PWM
    if (!InitPWM (&audio_format, producer_pool))
    {
        return false;
    }
#else
#error "No Audio Configuration specified"
#endif
    pAudioPool = producer_pool;
    
    return true;
}

bool CSoundDevice::InitI2S (audio_format_t *pAudioFormat, audio_buffer_pool *pBufferPool)
{
    // Basic hardware parameters (these are fixed for this implementation)
    uint8_t pio_sm=0;
    uint8_t dma_ch=0;
    uint8_t pin_data=I2S_DATA_PIN;
    uint8_t pin_bclk=I2S_BCLK_PIN;
    
    struct audio_i2s_config config = {
        .data_pin = pin_data,
        .clock_pin_base = pin_bclk,
        .dma_channel = dma_ch,
        .pio_sm = pio_sm,
    };

    const struct audio_format *output_format;
    output_format = audio_i2s_setup(pAudioFormat, &config);
    if (!output_format) {
        return false;
    }

    bool status = audio_i2s_connect(pBufferPool);
    if (!status) {
        return false;
    }

    audio_i2s_set_enabled(true);

    return true;
}

bool CSoundDevice::InitPWM (audio_format_t *pAudioFormat, audio_buffer_pool *pBufferPool)
{
    // Basic hardware parameters (these are fixed for this implementation)
    uint8_t pio_sm=0;
    uint8_t dma_ch=0;
    uint8_t base_pin=PWM_PIN;

    // This is essentially default_mono_channel_config
    // but using our specified PWM output pin.
    const audio_pwm_channel_config_t config =
    {
        .core = {
            .base_pin = base_pin,
            .dma_channel = dma_ch,
            .pio_sm = pio_sm
        },
        .pattern = 3,
    };

    const struct audio_format *output_format;
    output_format = audio_pwm_setup(pAudioFormat, -1, &config);    
    if (!output_format) {
        return false;
    }

    bool status = audio_pwm_default_connect(pBufferPool, false);
    if (!status) {
        return false;
    }

    audio_pwm_set_enabled(true);

    return true;
}

bool CSoundDevice::Update (buffer_callback cb) {
    // Grab a free pool from the audio layers for the new sample buffer
    struct audio_buffer *buffer = take_audio_buffer(pAudioPool, true);
    int16_t *samples = (int16_t *) buffer->buffer->bytes;
    
    // Use the callback to fill the buffer up to the maximum size permitted
    cb(samples, buffer->max_sample_count);
    buffer->sample_count = buffer->max_sample_count;
    
    // And now return the filled buffer to the pool for processing
    give_audio_buffer(pAudioPool, buffer);
    
    return true;
}
