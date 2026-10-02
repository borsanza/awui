# Herramientas de Chip-8 / MegaChip

- `dis.py`: desensamblador que sigue el flujo del programa (saltos, llamadas e instrucciones que se saltan) para separar
  código de datos. Uso: `python3 dis.py rom.ch8 > rom.asm`
- `asm.py`: ensamblador con etiquetas, `org`, `db`, `ds`, `incbin` y `equ`. Los mnemónicos son los del desensamblador,
  más `COL n` (color de colisión de MegaChip, `09nn`). Uso: `python3 asm.py fuente.src rom.ch8 [rom.map]`

## MegaBlinky v2

`megablinky2/build.sh` genera *MegaBlinky v2* a partir de la ROM original de MegaBlinky (2007, Revival Studios),
que es freeware. Ni la original ni la v2 están en el repositorio: la original se busca en la carpeta de ROMs de
StationTV (`~/.local/share/stationtv/roms/chip8/MegaChip8 Games`, o la que se pase como argumento) y la v2 se deja
junto a ella. Los cambios están en `megablinky2/v2.py` y descritos al principio del fuente que genera:

- Arreglado el fallo que impedía terminar los niveles: un punto comido con la pastilla activa sumaba 5 en vez de 1
  (seguía de largo hasta el código de la pastilla) y el contador se pasaba de los 247 que pide el nivel
- Marcador mientras se juega (puntuación, récord, vidas y nivel), y la puntuación sube con cada punto
- 3 vidas (antes 2) y una pausa al morir
- Fantasmas asustados en azul mientras dura la pastilla, parpadeando en blanco el último segundo
- Sonidos con el zumbador del Chip-8
- Quitada una tecla de depuración (F hacía más listos a los fantasmas)

El juego original necesita del emulador las colisiones de MegaChip con color (`09nn`) y los caracteres de la fuente
dibujados como sprites de un bit; los dos están en awui.
