#!/bin/bash
# Extract offset information from CUser getter/setter functions

# Get simple getters that directly return offset values
grep -E "^int __cdecl CUser::get_|^char __cdecl CUser::get_|^bool __cdecl CUser::get_|^unsigned __cdecl CUser::get_" df_game_r_v3.c -A 3 | \
grep -E "return \*\(\(|this \+ [0-9]+" | \
head -200
