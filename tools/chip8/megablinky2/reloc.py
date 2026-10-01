#!/usr/bin/env python3
"""Fuente reubicable: todos los datos con LDHI (24 bits) y la RAM del laberinto reservada en la ROM.
Así los datos pueden ir por encima de 0xFFF y queda sitio para código nuevo por debajo."""
s = open('megablinky.src').read()
s = s.replace('LD I, D_', 'LDHI I, D_')
s = s.replace('D_007458:                             ; fuera de la ROM (RAM)',
              'D_007458:                             ; laberinto en juego (RAM): 512 bytes\n    ds 512d')
open('reloc.src', 'w').write(s)
