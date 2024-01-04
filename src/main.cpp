#include "synth_dexed.h"

Dexed dexed(1,SAMPLE_RATE); // 1 voices max

int main(void)
{
  dexed.loadInitVoice();
  dexed.keydown(48, 100);
  dexed.keyup(48);
  
  return 0;
}
