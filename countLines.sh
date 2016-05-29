#!/bin/sh

find libawui/ -iregex '.*\.\(cpp\|h\)$' | xargs wc -l | sort -n

