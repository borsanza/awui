- Al no tener ni idea de como empezar, voy a recopilar todo lo que vaya leyendo para sacar conclusiones....

Sprites:
- Hay una zona de memoria para guardar sprites... Tiene de 0 a 447 sprites, osea 448
- Parece que cada sprite ocupa 0x20 bytes, osea 32 bytes.
- 4 de ancho x 8 de alto
- En resumen, consumira esta zona de memoria 448 x 32 = 14336 bytes = 14Kb (0x3800)

- En la wikipedia pone que tiene 64 sprites de 8x8, 16x8 o 16x16. Aun no entiendo esta parte para nada

Colores:
- Dos paletas de 16 colores que pueden ofrecer en la pantalla un total de 64 colores.
- 32 Colores (entre las dos paletas), 1 byte cada color. 2 bits menos significativos, rojo. 2 siguientes, verde y 2 siguientes azul, los dos mas altos no se usan.

00000000 = 0x00 = Negro
00110000 = 0x30 = Azul
00001100 = 0x0C = Verde
00000011 = 0x03 = Rojo
00111111 = 0x3F = Blanco

Roms:
- Pueden ser de 8 a 256Kb

RAM:
- 8 Kb

VRAM:
- 16 Kb

Graficos:
- 256 x 192 en las SG1000 / SC3000
- 248 x 192

- La pantalla es la composicion de dos capas, uno para sprites y otro para el fondo

CPU:
- La cpu es la de un zilog z80

  https://es.wikipedia.org/wiki/Zilog_Z80
  Registros:
	  - Banco principal A, B, C, D, E, H y L. Son de 8 bits
		- Banco secundario A', B', C', D', E', H' y L'
		- Registros especiales I, R, IX, IY, SP y PC

		Registro:
		  - A: Es un acumulador
			- R: Bloque de memoria a cuyo refresco se va a proceder
			- SP: Puntero de pila
			- PC: Contador de programa
			- F: Flags o bits de condicion
			- IX e IY: Registros indice
			- I: Registro de interrupciones


Memoria:
 - Ram:  0xC000 a 0xDFFF
 - VRam: 0x0000 a 0x3FFF
