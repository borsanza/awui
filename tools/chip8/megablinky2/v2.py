#!/usr/bin/env python3
"""MegaBlinky v2: cambios sobre el fuente reubicado (reloc.src) → megablinky2.src"""

src = open('reloc.src').read()
rom = open('megablinky.ch8', 'rb').read()


def rep(old, new, count=1):
    global src
    if src.count(old) != count:
        raise SystemExit('esperaba %d apariciones de:\n%s\n(hay %d)' % (count, old, src.count(old)))
    src = src.replace(old, new)


def strip_addr(block):
    """Quita los comentarios '; 2B3' del listado para poder buscar bloques."""
    import re
    return re.sub(r'\s+; [0-9A-F]{3}$', '', block, flags=re.M)


src = strip_addr(src)

# --- Cabecera
rep('; MegaBlinky (2007, Revival Studios), desensamblado y ensamblable con asm.py',
    '''; MegaBlinky v2
; Basado en MegaBlinky (2007, Revival Studios). Cambios de la v2:
;  - Arreglado: un punto comido con la pastilla activa contaba como pastilla (+4 y volvía a cargar el tiempo), el
;    contador se pasaba de 247 y el nivel ya no se podía terminar
;  - Marcador en pantalla mientras se juega: puntuación, récord, vidas y nivel. La puntuación sube con cada punto
;  - 3 vidas (antes 2), con una pausa al morir
;  - Fantasmas asustados en azul mientras dura la pastilla, parpadeando en blanco el último segundo
;  - Sonidos: puntos, pastillas, fantasmas, muerte y nivel completado
;  - Quitada la tecla F de depuración (hacía más listos a los fantasmas)''')

# --- Paleta del juego propia (colores 6 y 7 para los fantasmas asustados)
rep('''    LDHI I, D_007248
    LDPAL 08''', '''    LDHI I, gamepal
    LDPAL 08''', count=2)

# --- Partida nueva: 3 vidas y nivel 1
rep('''    LD V0, 05
    LDHI I, D_000947
    LD [I], V0-V0
    XOR V7, V7''', '''    LD V0, 05
    LDHI I, D_000947
    LD [I], V0-V0
    LD V0, 03
    LDHI I, lives
    LD [I], V0-V0
    LD V0, 01
    LDHI I, level
    LD [I], V0-V0
    XOR V7, V7''')

# --- Bucle principal: marcador tras el laberinto y fantasmas según la pastilla
rep('''L2B3:
    CLS
    SPRW 08
    SPRH 08
    CALL sub_7EF
    SPRW 10
    SPRH 10
    CALL sub_3E7
    LDHI I, D_000B48
    CALL sub_631
    COL 04
    CALL sub_75B
    SE VE, 00
    JP L301
    LDHI I, D_000E48
    CALL sub_511
    COL 05
    CALL sub_75B
    SE VE, 00
    JP L301
L2D9:''', '''L2B3:
    CLS
    SPRW 08
    SPRH 08
    CALL sub_7EF
    CALL hud
    SPRW 10
    SPRH 10
    CALL sub_3E7
    ; Con la pastilla (DT != 0) los fantasmas se pintan asustados: en azul y, el último segundo, parpadeando
    LD V0, DT
    SE V0, 00
    JP ghosts_scared
    LDHI I, D_000B48
    CALL sub_631
    COL 04
    CALL sub_75B
    SE VE, 00
    JP L301
    LDHI I, D_000E48
    CALL sub_511
    COL 05
    CALL sub_75B
    SE VE, 00
    JP L301
L2D9:''')

# --- Quitar la tecla F de depuración y terminar el nivel con >= 247 (por si acaso)
rep('''    LD V0, 0F
    SKNP V0
    CALL sub_851
    SE V6, F7
    JP L2B3
    LD VE, V6
    CALL sub_8F5
    LD VE, 64
    CALL sub_8F5
    CALL sub_851
    JP L295''', '''    LD V0, V6
    LD VE, F7
    SUB V0, VE
    SE VF, 01
    JP L2B3
    ; Nivel completado: +100, nivel siguiente, sonido y una pausa
    LD VE, 64
    CALL sub_8F5
    LDHI I, level
    LD V0-V0, [I]
    ADD V0, 01
    LD [I], V0-V0
    LD VE, 1E
    LD ST, VE
    LD VE, 5A
    CALL wait
    CALL sub_851
    JP L295''')

# --- Fantasma comido: sonido (los dos casos)
rep('''    LD VE, 32
    CALL sub_8F5''', '''    LD VE, 32
    CALL sub_8F5
    LD VE, 14
    LD ST, VE''')
rep('''    LD VE, 19
    CALL sub_8F5''', '''    LD VE, 19
    CALL sub_8F5
    LD VE, 14
    LD ST, VE''')

# --- Muerte: sonido, pausa y vidas en un contador (la puntuación ya va sumando, no se añade V6 al final)
rep('''L39D:
    LD VE, 40
    XOR V7, VE
    LD V0, V7
    AND V0, VE
    SE V0, 00
    JP L299
    LD VE, V6
    CALL sub_8F5
    CALL sub_905''', '''L39D:
    LD VE, 28
    LD ST, VE
    LD VE, 3C
    CALL wait
    LDHI I, lives
    LD V0-V0, [I]
    ADD V0, FF
    LD [I], V0-V0
    SE V0, 00
    JP L299
    CALL sub_905''')

# --- Punto comido: +1 al momento y sonido. Antes, con la pastilla activa seguía hasta el código de la pastilla
rep('''L4AB:
    LD VE, F0
    AND V0, VE
    OR V0, V3
    LD [I], V0-V0
    LDHI I, D_000C48
    SHL V4 {V0}
    SHL V5 {V0}
    SHR V4 {V0}
    SHR V5 {V0}
    ADD V6, 01
    LD V1, 05
    LD V0, DT
    SNE V0, 00
    JP L43B''', '''L4AB:
    LD VE, F0
    AND V0, VE
    OR V0, V3
    LD [I], V0-V0
    SHL V4 {V0}
    SHL V5 {V0}
    SHR V4 {V0}
    SHR V5 {V0}
    ADD V6, 01
    LD VE, 01
    CALL sub_8F5
    LD VE, 02
    LD ST, VE
    JP L43B''')

# --- Pastilla: +4 al momento y sonido
rep('''    ADD V6, 04''', '''    ADD V6, 04
    LD VE, 04
    CALL sub_8F5
    LD VE, 0A
    LD ST, VE''')


# --- Ritmo propio con DT; la pastilla pasa a la variable fright (en pasos)
rep('''L2B3:
    CLS
    SPRW 08''', '''L2B3:
    CALL step_wait
    CLS
    SPRW 08''')
rep('''    ; Con la pastilla (DT != 0) los fantasmas se pintan asustados: en azul y, el último segundo, parpadeando
    LD V0, DT''', '''    ; Con la pastilla (fright != 0) los fantasmas se pintan asustados: en azul y, al final, parpadeando
    CALL fright_v0''')
rep('''L301:
    LD V0, DT''', '''L301:
    CALL fright_v0''')
rep('''L501:
    LD V0, FF
    LD DT, V0''', '''L501:
    LD V0, 24
    LDHI I, fright
    LD [I], V0-V0''')
rep('''    LD VE, 80
    LD V1, DT''', '''    LD VE, 80
    CALL fright_v1''', count=2)
rep('''L299:
''', '''L299:
    XOR V0, V0
    LDHI I, fright
    LD [I], V0-V0
''')

# --- Rutinas nuevas y datos, antes de la zona de datos
new_code = r'''
; ---------------------------------------------------------------- v2: rutinas nuevas

; Fantasmas asustados: azules (color 6) o, los últimos pasos, parpadeando en blanco (color 7). El color de colisión
; es el mismo para los dos: al chocar, L301 mira a cuál de ellos se ha comido por la distancia
ghosts_scared:
    LD V1, V0
    LD VE, 09
    SUB V1, VE
    SE VF, 00
    JP ghosts_blue
    LD VE, 01
    AND V0, VE
    SE V0, 00
    JP ghosts_white
ghosts_blue:
    LDHI I, ghost_blue
    CALL sub_631
    COL 06
    CALL sub_75B
    SE VE, 00
    JP L301
    LDHI I, ghost_blue
    CALL sub_511
    COL 06
    CALL sub_75B
    SE VE, 00
    JP L301
    JP L2D9
ghosts_white:
    LDHI I, ghost_white
    CALL sub_631
    COL 07
    CALL sub_75B
    SE VE, 00
    JP L301
    LDHI I, ghost_white
    CALL sub_511
    COL 07
    CALL sub_75B
    SE VE, 00
    JP L301
    JP L2D9

; Ritmo del juego: espera a que pase el paso anterior (DT) y descuenta un paso a la pastilla
step_wait:
    LD VE, DT
    SE VE, 00
    JP step_wait
    LD VE, 07
    LD DT, VE
    LDHI I, fright
    LD V0-V0, [I]
    SE V0, 00
    ADD V0, FF
    LD [I], V0-V0
    RET

; V0 = pasos que quedan de pastilla (0: no hay)
fright_v0:
    LDHI I, fright
    LD V0-V0, [I]
    RET

; V1 = pasos que quedan de pastilla, sin tocar V0 (la IA de los fantasmas lo tiene ocupado)
fright_v1:
    LDHI I, tmp0
    LD [I], V0-V0
    LDHI I, fright
    LD V0-V0, [I]
    LD V1, V0
    LDHI I, tmp0
    LD V0-V0, [I]
    RET

; Espera VE sesentavos de segundo (con DT)
wait:
    LD DT, VE
wait_loop:
    LD VE, DT
    SE VE, 00
    JP wait_loop
    RET

; Marcador: puntuación (arriba a la izquierda), récord (arriba a la derecha), vidas que quedan (abajo a la
; izquierda) y nivel (abajo a la derecha). Guarda y recupera todos los registros
hud:
    LDHI I, regs
    LD [I], V0-VF
    LD V6, 08
    LD V7, 0B
    LDHI I, D_000943
    CALL sub_861
    LD V6, A8
    LD V7, 0B
    LDHI I, D_000945
    CALL sub_861
    ; Nivel, con dos cifras
    LDHI I, level
    LD V0-V0, [I]
    LDHI I, bcd
    LD B, V0
    LD V0-V2, [I]
    LD V6, E0
    LD V7, AB
    LD HF, V1
    DRW V6, V7, A
    ADD V6, 0C
    LD HF, V2
    DRW V6, V7, A
    ; Vidas: un comecocos por cada vida que queda además de la que se juega
    SPRW 10
    SPRH 10
    LDHI I, lives
    LD V0-V0, [I]
    LD V3, V0
    ADD V3, FF
    LD V6, 08
    LD V7, A8
hud_lives:
    SE V3, 00
    JP hud_life
    JP hud_end
hud_life:
    LDHI I, D_001348+400
    DRW V6, V7, 1
    ADD V6, 14
    ADD V3, FF
    JP hud_lives
hud_end:
    LDHI I, regs
    LD V0-VF, [I]
    RET
'''
marker = '\n; ---------------------------------------------------------------- datos'
src = src.replace(marker, new_code + marker, 1)

# Datos nuevos: paleta del juego (los 5 primeros colores del original, 6 = azul asustado, 7 = blanco), fantasmas
# asustados (la forma del fantasma naranja con otro color) y variables
pal = bytearray(rom[0x7248 - 0x200:0x7248 - 0x200 + 32])
pal[20:24] = bytes([0xFF, 0x21, 0x21, 0xFF])   # 6: azul
pal[24:28] = bytes([0xFF, 0xFF, 0xFF, 0xFF])   # 7: blanco
ghost = rom[0xE48 - 0x200:0xE48 - 0x200 + 256]
blue = bytes(6 if b == 5 else b for b in ghost)
white = bytes(7 if b == 5 else b for b in ghost)


def db(name, data):
    lines = ['%s:' % name]
    for i in range(0, len(data), 16):
        lines.append('    db ' + ', '.join('0x%02X' % b for b in data[i:i + 16]))
    return '\n'.join(lines)


extra = '\n'.join([
    '',
    '; ---------------------------------------------------------------- v2: datos',
    db('gamepal', pal),
    db('ghost_blue', blue),
    db('ghost_white', white),
    'lives:',
    '    db 03',
    'level:',
    '    db 01',
    'fright:',
    '    db 00',
    'tmp0:',
    '    db 00',
    'bcd:',
    '    ds 3d',
    'regs:',
    '    ds 16d',
    ''])
src = src.rstrip('\n') + '\n' + extra
open('megablinky2.src', 'w').write(src)
print('megablinky2.src escrito')
