#!/bin/sh
valgrind --tool=callgrind ./awMasterGear ./tests/64*.sms
