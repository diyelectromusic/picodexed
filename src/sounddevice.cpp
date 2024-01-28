#include "pico/audio_i2s.h"
#include "sounddevice.h"

#define SAMPLES_PER_BUFFER 256

CSoundDevice::CSoundDevice (void)
:
	pAudioPool(nullptr)
{
}

CSoundDevice::~CSoundDevice (void)
{
}

bool CSoundDevice::Init (uint32_t sample_rate, uint8_t pin_data, uint8_t pin_bclk) {
	struct audio_buffer_pool * pAudioPool;
	
	// Basic hardware parameters (these are fixed for this implementation)
	uint8_t pio_sm=0;
	uint8_t dma_ch=0;
	uint16_t num_samples=SAMPLES_PER_BUFFER;
	
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
		num_samples
	);

	const struct audio_format *output_format;

	struct audio_i2s_config config = {
		.data_pin = pin_data,
		.clock_pin_base = pin_bclk,
		.dma_channel = dma_ch,
		.pio_sm = pio_sm,
	};

	output_format = audio_i2s_setup(&audio_format, &config);
	if (!output_format) {
		return false;
		//panic("PicoAudio: Unable to open audio device.\n");
	}

	bool status = audio_i2s_connect(producer_pool);
	if (!status) {
		return false;
		//panic("PicoAudio: Unable to connect to audio device.\n");
	}

	audio_i2s_set_enabled(true);

	pAudioPool = producer_pool;
	
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
