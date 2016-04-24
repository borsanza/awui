#!/bin/sh

#vblank_mode=0 ./awMasterGear tests/*
valgrind --tool=callgrind ./awMasterGear --test zexall/zexall_sdsc.sms

# Despues ejecutamos
# callgrind_annotate callgrind.out.<pid>

