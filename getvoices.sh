#!/bin/sh

# Get voices from
# https://yamahablackboxes.com/collection/yamaha-dx7-synthesizer/patches/

mkdir -p voices

DIR="https://yamahablackboxes.com/patches/dx7/factory"

wget -c "${DIR}"/rom1a.syx -O voices/rom1a.syx
wget -c "${DIR}"/rom1b.syx -O voices/rom1b.syx
wget -c "${DIR}"/rom2a.syx -O voices/rom2a.syx
wget -c "${DIR}"/rom2b.syx -O voices/rom2b.syx
wget -c "${DIR}"/rom3a.syx -O voices/rom3a.syx
wget -c "${DIR}"/rom3b.syx -O voices/rom3b.syx
wget -c "${DIR}"/rom4a.syx -O voices/rom4a.syx
wget -c "${DIR}"/rom4b.syx -O voices/rom4b.syx

./synth_dexed/Synth_Dexed/tools/sysex2c.py voices/* > src/voices.h
