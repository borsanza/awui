#!/bin/sh

#vblank_mode=0 ./stationTV roms/mastersystem/tests/*
valgrind --tool=callgrind ./stationTV --test roms/mastersystem/zexall/zexall_sdsc.sms

# Despues ejecutamos
# callgrind_annotate callgrind.out.<pid>

