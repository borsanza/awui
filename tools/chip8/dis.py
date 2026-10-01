#!/usr/bin/env python3
"""Desensamblador de CHIP-8 / SuperChip / MegaChip que sigue el flujo (saltos, llamadas, skips)."""
import sys

ROM = sys.argv[1]
data = open(ROM, 'rb').read()
BASE = 0x200
mem = bytearray(BASE) + bytearray(data)


def word(a):
    return (mem[a] << 8) | mem[a + 1]


def decode(a):
    op = word(a)
    n1, x, y, n = op >> 12, (op >> 8) & 0xF, (op >> 4) & 0xF, op & 0xF
    nn, nnn = op & 0xFF, op & 0xFFF
    nxt = [a + 2]
    size = 2
    s = '???  %04X' % op
    if op == 0x00E0: s = 'CLS'
    elif op == 0x00EE: s = 'RET'; nxt = []
    elif op == 0x00FB: s = 'SCR'
    elif op == 0x00FC: s = 'SCL'
    elif op == 0x00FD: s = 'EXIT'; nxt = []
    elif op == 0x00FE: s = 'LOW'
    elif op == 0x00FF: s = 'HIGH'
    elif op == 0x0010: s = 'MEGAOFF'
    elif op == 0x0011: s = 'MEGAON'
    elif (op & 0xFFF0) == 0x00B0: s = 'SCRU %X' % n
    elif (op & 0xFFF0) == 0x00C0: s = 'SCD %X' % n
    elif (op & 0xFF00) == 0x0100:
        s = 'LDHI I, %06X' % ((nn << 16) | word(a + 2)); size = 4; nxt = [a + 4]
    elif (op & 0xFF00) == 0x0200: s = 'LDPAL %02X' % nn
    elif (op & 0xFF00) == 0x0300: s = 'SPRW %02X' % nn
    elif (op & 0xFF00) == 0x0400: s = 'SPRH %02X' % nn
    elif (op & 0xFF00) == 0x0500: s = 'ALPHA %02X' % nn
    elif (op & 0xFFF0) == 0x0600: s = 'DIGISND %X' % n
    elif op == 0x0700: s = 'STOPSND'
    elif (op & 0xFFF0) == 0x0800: s = 'BMODE %X' % n
    elif n1 == 0x1: s = 'JP %03X' % nnn; nxt = [nnn]
    elif n1 == 0x2: s = 'CALL %03X' % nnn; nxt = [a + 2, nnn]
    elif n1 == 0x3: s = 'SE V%X, %02X' % (x, nn); nxt = [a + 2, a + 4]
    elif n1 == 0x4: s = 'SNE V%X, %02X' % (x, nn); nxt = [a + 2, a + 4]
    elif n1 == 0x5 and n == 0: s = 'SE V%X, V%X' % (x, y); nxt = [a + 2, a + 4]
    elif n1 == 0x6: s = 'LD V%X, %02X' % (x, nn)
    elif n1 == 0x7: s = 'ADD V%X, %02X' % (x, nn)
    elif n1 == 0x8:
        s = {0: 'LD V%X, V%X', 1: 'OR V%X, V%X', 2: 'AND V%X, V%X', 3: 'XOR V%X, V%X', 4: 'ADD V%X, V%X',
             5: 'SUB V%X, V%X', 6: 'SHR V%X {V%X}', 7: 'SUBN V%X, V%X', 0xE: 'SHL V%X {V%X}'}.get(n, '??? %04X' % op)
        if '%' in s: s = s % (x, y)
    elif n1 == 0x9 and n == 0: s = 'SNE V%X, V%X' % (x, y); nxt = [a + 2, a + 4]
    elif n1 == 0xA: s = 'LD I, %03X' % nnn
    elif n1 == 0xB: s = 'JP V0, %03X' % nnn; nxt = []
    elif n1 == 0xC: s = 'RND V%X, %02X' % (x, nn)
    elif n1 == 0xD: s = 'DRW V%X, V%X, %X' % (x, y, n)
    elif n1 == 0xE and nn == 0x9E: s = 'SKP V%X' % x; nxt = [a + 2, a + 4]
    elif n1 == 0xE and nn == 0xA1: s = 'SKNP V%X' % x; nxt = [a + 2, a + 4]
    elif n1 == 0xF:
        s = {0x07: 'LD V%X, DT', 0x0A: 'LD V%X, K', 0x15: 'LD DT, V%X', 0x18: 'LD ST, V%X', 0x1E: 'ADD I, V%X',
             0x29: 'LD F, V%X', 0x30: 'LD HF, V%X', 0x33: 'LD B, V%X', 0x55: 'LD [I], V0-V%X', 0x65: 'LD V0-V%X, [I]',
             0x75: 'LD R, V0-V%X', 0x85: 'LD V0-V%X, R'}.get(nn, '??? %04X' % op)
        if '%' in s: s = s % x
    # Tras un skip, la instrucción saltada puede ser un LDHI de 4 bytes
    if s.startswith(('SE', 'SNE', 'SKP', 'SKNP')) and (word(a + 2) & 0xFF00) == 0x0100:
        nxt = [a + 2, a + 6]
    return s, size, nxt


code = {}
calls = set()
jumps = set()
todo = [BASE]
while todo:
    a = todo.pop()
    if a in code or a < BASE or a + 1 >= len(mem):
        continue
    s, size, nxt = decode(a)
    code[a] = (s, size)
    if s.startswith('CALL'): calls.add(int(s.split()[1], 16))
    if s.startswith('JP ') and not s.startswith('JP V0'): jumps.add(int(s.split()[1], 16))
    todo.extend(nxt)

a = BASE
end = max(code) + 4 if code else BASE
while a < end:
    if a in code:
        s, size = code[a]
        label = ''
        if a in calls: label = 'sub_%03X:' % a
        elif a in jumps: label = 'L%03X:' % a
        if label: print(label)
        raw = ' '.join('%02X' % b for b in mem[a:a + size])
        print('  %03X  %-11s  %s' % (a, raw, s))
        a += size
    else:
        start = a
        while a < end and a not in code:
            a += 1
        print('  %03X  ; datos: %d bytes' % (start, a - start))
print('; código hasta %03X; ROM hasta %06X' % (end, BASE + len(data)), file=sys.stderr)
print('; %d instrucciones, %d subrutinas' % (len(code), len(calls)), file=sys.stderr)
