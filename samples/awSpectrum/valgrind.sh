#!/bin/sh

#vblank_mode=0 ./awSpectrum roms/mastersystem/tests/*
valgrind --tool=callgrind ./awSpectrum --test roms/mastersystem/zexall/zexall_sdsc.sms

# Despues ejecutamos
# callgrind_annotate callgrind.out.<pid>

