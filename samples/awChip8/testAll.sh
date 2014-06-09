#!/bin/sh
find roms/*.ch8 | xargs -i sudo ./awChip8 {}
