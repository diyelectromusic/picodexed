#ifdef __cplusplus
extern "C" {
#endif

#include "tusb.h"

#define MIDI_USB_PKT_SIZE 4
void usbMidiTaskInit (void);
uint32_t usbMidiTaskProcess (void);
uint32_t usbMidiTaskRead (uint8_t *pDataStream, unsigned nStreamSize);

#ifdef __cplusplus
}
#endif
