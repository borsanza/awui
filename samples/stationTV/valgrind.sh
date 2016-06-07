#!/bin/sh

#vblank_mode=0 ./awArcade roms/mastersystem/tests/*
valgrind --tool=callgrind ./awArcade --test roms/mastersystem/zexall/zexall_sdsc.sms

# Despues ejecutamos
# callgrind_annotate callgrind.out.<pid>

