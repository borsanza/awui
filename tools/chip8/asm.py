#!/usr/bin/env python3
"""Ensamblador sencillo de CHIP-8 / SuperChip / MegaChip.

Sintaxis (una instrucción por línea, ';' comentario):
    etiqueta:
    org 0x200            ; sitúa lo siguiente en esa dirección (solo hacia delante; rellena con ceros)
    db 0x12, 34, ...     ; bytes
    incbin "f.bin", ini, n   ; n bytes de un fichero desde el desplazamiento ini
    equ NOMBRE, valor    ; constante
    mnemónicos como los del desensamblador: LD V1, 18 / JP etiqueta / LDHI I, etiqueta / DRW V1, V2, 1 ...
Los números sin prefijo en instrucciones son hexadecimales (como en el listado); con 0x o # también, y con
el sufijo 'd' decimales (12d).
"""
import re
import sys

REG = re.compile(r'^V([0-9A-F])$', re.I)


class Asm:
    def __init__(self):
        self.labels = {}
        self.equs = {}

    def num(self, tok, pc=None, size=None):
        tok = tok.strip()
        if tok.upper() in self.equs:
            return self.equs[tok.upper()]
        if tok in self.labels:
            return self.labels[tok]
        if re.fullmatch(r'[0-9]+d', tok, re.I):
            return int(tok[:-1])
        if tok.startswith(('0x', '0X')):
            return int(tok, 16)
        if tok.startswith('#'):
            return int(tok[1:], 16)
        if re.fullmatch(r'[0-9A-Fa-f]+', tok):
            return int(tok, 16)
        m = re.fullmatch(r'(\w+)\s*([+-])\s*(\w+)', tok)
        if m:
            a = self.num(m.group(1)); b = self.num(m.group(3))
            return a + b if m.group(2) == '+' else a - b
        if self.pass2:
            raise ValueError('símbolo desconocido: %s' % tok)
        return 0

    def reg(self, tok):
        m = REG.match(tok.strip())
        if not m:
            raise ValueError('se esperaba un registro: %s' % tok)
        return int(m.group(1), 16)

    def encode(self, mn, args, pc):
        mn = mn.upper()
        a = [x.strip() for x in args]
        A = [x.upper() for x in a]
        if mn == 'CLS': return [0x00E0]
        if mn == 'RET': return [0x00EE]
        if mn == 'SCR': return [0x00FB]
        if mn == 'SCL': return [0x00FC]
        if mn == 'EXIT': return [0x00FD]
        if mn == 'LOW': return [0x00FE]
        if mn == 'HIGH': return [0x00FF]
        if mn == 'MEGAOFF': return [0x0010]
        if mn == 'MEGAON': return [0x0011]
        if mn == 'SCRU': return [0x00B0 | self.num(a[0])]
        if mn == 'SCD': return [0x00C0 | self.num(a[0])]
        if mn == 'LDHI':
            v = self.num(a[1]); return [0x0100 | ((v >> 16) & 0xFF), v & 0xFFFF]
        if mn == 'LDPAL': return [0x0200 | self.num(a[0])]
        if mn == 'SPRW': return [0x0300 | (self.num(a[0]) & 0xFF)]
        if mn == 'SPRH': return [0x0400 | (self.num(a[0]) & 0xFF)]
        if mn == 'ALPHA': return [0x0500 | self.num(a[0])]
        if mn == 'DIGISND': return [0x0600 | self.num(a[0])]
        if mn == 'STOPSND': return [0x0700]
        if mn == 'BMODE': return [0x0800 | self.num(a[0])]
        if mn == 'COL': return [0x0900 | self.num(a[0])]          # color de colisión (09nn)
        if mn == 'RAW': return [self.num(a[0])]
        if mn == 'JP':
            if len(a) == 2: return [0xB000 | (self.num(a[1]) & 0xFFF)]
            v = self.num(a[0]); self.check12(v, pc); return [0x1000 | v]
        if mn == 'CALL':
            v = self.num(a[0]); self.check12(v, pc); return [0x2000 | v]
        if mn in ('SE', 'SNE'):
            x = self.reg(a[0])
            if REG.match(a[1]):
                return [(0x5000 if mn == 'SE' else 0x9000) | (x << 8) | (self.reg(a[1]) << 4)]
            return [(0x3000 if mn == 'SE' else 0x4000) | (x << 8) | (self.num(a[1]) & 0xFF)]
        if mn == 'LD':
            if A[0] == 'I':
                v = self.num(a[1]); self.check12(v, pc); return [0xA000 | v]
            if A[0] == 'DT': return [0xF015 | (self.reg(a[1]) << 8)]
            if A[0] == 'ST': return [0xF018 | (self.reg(a[1]) << 8)]
            if A[0] == 'F': return [0xF029 | (self.reg(a[1]) << 8)]
            if A[0] == 'HF': return [0xF030 | (self.reg(a[1]) << 8)]
            if A[0] == 'B': return [0xF033 | (self.reg(a[1]) << 8)]
            if A[0] == '[I]': return [0xF055 | (self.reg(a[1].split('-')[1]) << 8)]
            if A[0] == 'R': return [0xF075 | (self.reg(a[1].split('-')[1]) << 8)]
            if '-' in A[0]:
                x = self.reg(a[0].split('-')[1])
                if A[1] == '[I]': return [0xF065 | (x << 8)]
                if A[1] == 'R': return [0xF085 | (x << 8)]
            x = self.reg(a[0])
            if A[1] == 'DT': return [0xF007 | (x << 8)]
            if A[1] == 'K': return [0xF00A | (x << 8)]
            if REG.match(a[1]): return [0x8000 | (x << 8) | (self.reg(a[1]) << 4)]
            return [0x6000 | (x << 8) | (self.num(a[1]) & 0xFF)]
        if mn == 'ADD':
            if A[0] == 'I': return [0xF01E | (self.reg(a[1]) << 8)]
            x = self.reg(a[0])
            if REG.match(a[1]): return [0x8004 | (x << 8) | (self.reg(a[1]) << 4)]
            return [0x7000 | (x << 8) | (self.num(a[1]) & 0xFF)]
        ops = {'OR': 1, 'AND': 2, 'XOR': 3, 'SUB': 5, 'SHR': 6, 'SUBN': 7, 'SHL': 0xE}
        if mn in ops:
            x = self.reg(a[0]); y = self.reg(a[1].strip('{} ')) if len(a) > 1 else x
            return [0x8000 | (x << 8) | (y << 4) | ops[mn]]
        if mn == 'RND': return [0xC000 | (self.reg(a[0]) << 8) | (self.num(a[1]) & 0xFF)]
        if mn == 'DRW': return [0xD000 | (self.reg(a[0]) << 8) | (self.reg(a[1]) << 4) | (self.num(a[2]) & 0xF)]
        if mn == 'SKP': return [0xE09E | (self.reg(a[0]) << 8)]
        if mn == 'SKNP': return [0xE0A1 | (self.reg(a[0]) << 8)]
        raise ValueError('instrucción desconocida: %s %s' % (mn, ','.join(args)))

    def check12(self, v, pc):
        if self.pass2 and v > 0xFFF:
            raise ValueError('dirección %X fuera de los 12 bits (en %X): usa LDHI' % (v, pc))

    def split_args(self, rest):
        # SHR V0 {V0} → dos args; "LD V0-V3, [I]" → dos
        rest = rest.replace('{', ',{')
        return [x for x in (p.strip() for p in rest.split(',')) if x]

    def assemble(self, lines, base=0x200):
        for self.pass2 in (False, True):
            pc = base
            out = bytearray()
            for lineno, raw in enumerate(lines, 1):
                line = raw.split(';', 1)[0].strip()
                if not line:
                    continue
                try:
                    m = re.match(r'^([A-Za-z_][\w.]*):\s*(.*)$', line)
                    if m:
                        if not self.pass2:
                            if m.group(1) in self.labels:
                                raise ValueError('etiqueta repetida: %s' % m.group(1))
                            self.labels[m.group(1)] = pc
                        line = m.group(2).strip()
                        if not line:
                            continue
                    parts = line.split(None, 1)
                    mn = parts[0]; rest = parts[1] if len(parts) > 1 else ''
                    if mn.lower() == 'equ':
                        name, val = [x.strip() for x in rest.split(',', 1)]
                        self.equs[name.upper()] = self.num(val); continue
                    if mn.lower() == 'org':
                        target = self.num(rest)
                        if target < pc:
                            raise ValueError('org %X hacia atrás (pc=%X)' % (target, pc))
                        out += bytes(target - pc); pc = target; continue
                    if mn.lower() == 'db':
                        vals = [self.num(v) & 0xFF for v in rest.split(',') if v.strip()]
                        out += bytes(vals); pc += len(vals); continue
                    if mn.lower() == 'ds':
                        n = self.num(rest); out += bytes(n); pc += n; continue
                    if mn.lower() == 'incbin':
                        f, ini, n = [x.strip() for x in rest.split(',')]
                        blob = open(f.strip('"'), 'rb').read()[self.num(ini):self.num(ini) + self.num(n)]
                        out += blob; pc += len(blob); continue
                    words = self.encode(mn, self.split_args(rest), pc)
                    for w in words:
                        out += bytes([(w >> 8) & 0xFF, w & 0xFF]); pc += 2
                except ValueError as e:
                    raise SystemExit('línea %d: %s\n  %s' % (lineno, e, raw.rstrip()))
        return bytes(out)


if __name__ == '__main__':
    src, dst = sys.argv[1], sys.argv[2]
    a = Asm()
    rom = a.assemble(open(src).read().split('\n'))
    open(dst, 'wb').write(rom)
    print('%s: %d bytes (0x200-0x%X)' % (dst, len(rom), 0x200 + len(rom)))
    if len(sys.argv) > 3:
        with open(sys.argv[3], 'w') as f:
            for k, v in sorted(a.labels.items(), key=lambda kv: kv[1]):
                f.write('%06X %s\n' % (v, k))
