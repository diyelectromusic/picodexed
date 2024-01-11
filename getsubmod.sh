cd ../synth_dexed
git clone https://codeberg.org/dcoredump/Synth_Dexed.git
echo "$(echo "#include \"dexed_if_common.h\""; cat Synth_Dexed/src/dexed.h)" > Synth_Dexed/src/dexed.h
cd ..

