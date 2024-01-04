Directory for the port of Synth_Dexed from https://codeberg.org/dcoredump/Synth_Dexed

Note: that Synth_Dexed/src/dexed.h will need to add the following line at the top:
#include "dexed_if_common.h"

This provides a few things that Synth_Dexed requires that aren't provided by default by the SDK.
