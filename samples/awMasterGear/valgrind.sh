#!/bin/sh

#vblank_mode=0 ./awMasterGear tests/*
valgrind --tool=callgrind ./awMasterGear ./tests/64*.sms
