#!/usr/bin/env python3
"""Convierte el listado desensamblado en un fuente ensamblable con etiquetas."""
import re

ROMFILE = 'megablinky.ch8'
rom = open(ROMFILE, 'rb').read()
END = 0x200 + len(rom)
listing = open('megablinky.asm').read().split('\n')

code = []          # (addr, raw, text)
datablocks = []    # (addr, n)
for line in listing:
    m = re.match(r'^\s+([0-9A-F]{3})\s+((?:[0-9A-F]{2} ?)+)\s+(.*)$', line)
    if m:
        code.append((int(m.group(1), 16), m.group(2).split(), m.group(3).strip()))
        continue
    m = re.match(r'^\s+([0-9A-F]{3})\s+; datos: (\d+) bytes', line)
    if m:
        datablocks.append((int(m.group(1), 16), int(m.group(2))))

# Destinos de saltos y llamadas
targets = {}
for a, raw, t in code:
    m = re.match(r'^(JP|CALL) ([0-9A-F]{3})$', t)
    if m:
        dst = int(m.group(2), 16)
        targets.setdefault(dst, 'sub_%03X' % dst if m.group(1) == 'CALL' else 'L%03X' % dst)
        if m.group(1) == 'CALL':
            targets[dst] = 'sub_%03X' % dst

# Direcciones de datos que usa el código
datarefs = set()
for a, raw, t in code:
    m = re.match(r'^LD I, ([0-9A-F]{3})$', t)
    if m: datarefs.add(int(m.group(1), 16))
    m = re.match(r'^LDHI I, ([0-9A-F]{6})$', t)
    if m: datarefs.add(int(m.group(1), 16))
code_end = max(a + len(r) for a, r, t in code)
datarefs = {d for d in datarefs if d >= code_end}


def dlabel(addr):
    return 'D_%06X' % addr


out = []
out.append('; MegaBlinky (2007, Revival Studios), desensamblado y ensamblable con asm.py')
out.append('')
codeaddrs = {a for a, r, t in code}
for a, raw, t in code:
    if a in targets:
        out.append('%s:' % targets[a])
    m = re.match(r'^(JP|CALL) ([0-9A-F]{3})$', t)
    if m:
        t = '%s %s' % (m.group(1), targets[int(m.group(2), 16)])
    m = re.match(r'^LD I, ([0-9A-F]{3})$', t)
    if m and int(m.group(1), 16) in datarefs:
        t = 'LD I, %s' % dlabel(int(m.group(1), 16))
    m = re.match(r'^LDHI I, ([0-9A-F]{6})$', t)
    if m and int(m.group(1), 16) in datarefs:
        t = 'LDHI I, %s' % dlabel(int(m.group(1), 16))
    m = re.match(r'^\?\?\?\s+09([0-9A-F]{2})$', t)
    if m:
        t = 'COL %s' % m.group(1)
    elif t.startswith('???'):
        t = 'RAW %s' % t.split()[1]
    out.append('    %-28s ; %03X' % (t, a))
    # Bloques de datos dentro de la zona de código (cabecera tras el salto inicial)
    nxt = a + len(raw)
    for da, n in datablocks:
        if da == nxt and da < code_end:
            out.append('    incbin "%s", %s, %dd        ; %03X: datos' % (ROMFILE, hex(da - 0x200), n, da))

# Zona de datos: de code_end al final, partida en las direcciones referenciadas
cuts = sorted({code_end, END} | datarefs)
out.append('')
out.append('; ---------------------------------------------------------------- datos')
for start, stop in zip(cuts, cuts[1:]):
    if start >= END:
        break
    if start in datarefs:
        out.append('%s:' % dlabel(start))
    out.append('    incbin "%s", %s, %dd' % (ROMFILE, hex(start - 0x200), min(stop, END) - start))
for d in sorted(datarefs):
    if d >= END:
        out.append('%s:                             ; fuera de la ROM (RAM)' % dlabel(d))
open('megablinky.src', 'w').write('\n'.join(out) + '\n')
print('código hasta %X, %d referencias a datos: %s' % (code_end, len(datarefs), ' '.join('%X' % d for d in sorted(datarefs))))
