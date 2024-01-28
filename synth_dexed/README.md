Directory for the port of Synth_Dexed from https://codeberg.org/dcoredump/Synth_Dexed

This provides a few things that Synth_Dexed requires that aren't provided by default by the SDK.

The following files need to be updated:

src/dexed.cpp:
See the file "dexed.diff" in this repository for the changes required to dexed.cpp.

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
