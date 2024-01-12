Directory for the port of Synth_Dexed from https://codeberg.org/dcoredump/Synth_Dexed

This provides a few things that Synth_Dexed requires that aren't provided by default by the SDK.

The following files need to be updated:

src/dexed.h:
```
--- a/src/dexed.h
+++ b/src/dexed.h
@@ -47,6 +47,9 @@
 #include "EngineMsfa.h"
 #include "EngineMkI.h"
 #include "EngineOpl.h"
+#ifdef RASPBERRYPI_PICO
+#include "dexed_if_common.h"
+#endif
 
 #define NUM_VOICE_PARAMETERS 156
 
@@ -383,8 +386,10 @@ class Dexed
     uint8_t velocity_max;
     float velocity_diff;
 #ifndef TEENSYDUINO
+#ifndef RASPBERRYPI_PICO
     Compressor* compressor;
 #endif
+#endif
 };
 
 #endif
```

src/dexed.cpp
```
--- a/src/dexed.cpp
+++ b/src/dexed.cpp
@@ -80,7 +80,9 @@ Dexed::Dexed(uint8_t maxnotes, int rate)
   setEngineType(MKI);
 
 #ifndef TEENSYDUINO
+#ifndef RASPBERRYPI_PICO
   compressor = new Compressor(samplerate);
+#endif
 #endif
   use_compressor = false;
 }
@@ -231,9 +233,11 @@ void Dexed::getSamples(float32_t* buffer, uint16_t n_samples)
   fx.process(buffer, n_samples); // Needed for fx.Gain()!!!
 
 #ifndef TEENSYDUINO
+#ifndef RASPBERRYPI_PICO
   if (use_compressor == true)
     compressor->doCompression(buffer, n_samples);
 #endif
+#endif
 }
 
 void Dexed::getSamples(int16_t* buffer, uint16_t n_samples)
@@ -1754,6 +1758,7 @@ void Dexed::setVelocityScale(uint8_t setup = MIDI_VELOCITY_SCALING_OFF)
 }
 
 #ifndef TEENSYDUINO
+#ifndef RASPBERRYPI_PICO
 void Dexed::setCompressor(bool enable_compressor)
 {
   use_compressor = enable_compressor;
@@ -1814,3 +1819,4 @@ float32_t Dexed::getCompressionRatio(void)
   return (compressor->getCompressionRatio());
 }
 #endif
+#endif
```

src/compressor.h
```
--- a/src/compressor.h
+++ b/src/compressor.h
@@ -16,6 +16,7 @@
 #define _COMPRESSOR_H
 
 #ifndef TEENSYDUINO
+#ifndef RASPBERRYPI_PICO
 
 #include <arm_math.h> //ARM DSP extensions.  https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html
 #include "synth.h"
@@ -423,6 +424,9 @@ class Compressor
     
 };
 
+#else
+#warning USING RASPBERRYPI_PICO SO INTERNAL COMPRESSOR IS DISABLED!
+#endif // RASPBERRYPI_PICO
 #else
 #warning USING TEENSYDUINO SO INTERNAL COMPRESSOR IS DISABLED!
 #endif // TEENSYDUINO
```
