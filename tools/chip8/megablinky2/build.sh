#!/bin/sh
# Genera "MegaBlinky v2" a partir de la ROM original de MegaBlinky (2007, Revival Studios):
#   desensambla, comprueba que el fuente vuelve a dar la ROM original byte a byte, reubica los datos y aplica v2.py
# La ROM original no está en el repositorio (no se puede redistribuir): se busca en la carpeta de ROMs del usuario de
# StationTV, o en la que se pase como argumento. La v2 se deja en la misma carpeta
#   build.sh ["carpeta con MegaBlinky (2007) [Revival Studios].ch8"]
set -e
ROMS=$(realpath "${1:-${XDG_DATA_HOME:-$HOME/.local/share}/stationtv/roms/chip8/MegaChip8 Games}")
cd "$(dirname "$0")"
mkdir -p out && cd out
cp "$ROMS/MegaBlinky (2007) [Revival Studios].ch8" megablinky.ch8
python3 ../../dis.py megablinky.ch8 > megablinky.asm
python3 ../gen.py
python3 ../../asm.py megablinky.src roundtrip.ch8 > /dev/null
cmp roundtrip.ch8 megablinky.ch8 && echo "Fuente comprobado: vuelve a dar la ROM original"
python3 ../reloc.py
python3 ../v2.py
python3 ../../asm.py megablinky2.src megablinky2.ch8 megablinky2.map
cp megablinky2.ch8 "$ROMS/MegaBlinky v2 (2026) [Revival Studios, mod].ch8"
echo "Copiada a $ROMS"
