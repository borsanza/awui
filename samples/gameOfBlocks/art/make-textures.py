#!/usr/bin/env python3
"""Genera las texturas de los bloques de gameOfBlocks (16x16, PNG).

Son propias: salen de este script (ruido y formas sencillas con una semilla fija), no de ningún juego. Para cambiarlas,
se toca aquí y se vuelven a generar:

    samples/gameOfBlocks/art/make-textures.py build/samples/gameOfBlocks/images

Las de prueba (block-empty.png y block-pattern-*.png) no se generan aquí: están dibujadas a mano.
"""

import os
import random
import sys

from PIL import Image

SIZE = 16


def noise(rng, base, spread, tint=(1.0, 1.0, 1.0)):
    """Un color alrededor de base, más claro o más oscuro al azar."""
    delta = rng.randint(-spread, spread)
    return tuple(max(0, min(255, int(c + delta * t))) for c, t in zip(base, tint))


def speckled(seed, base, spread, specks=(), tint=(1.0, 1.0, 1.0)):
    """Fondo con grano y, encima, motas sueltas: specks es una lista de (color, cuántas)."""
    rng = random.Random(seed)
    image = Image.new("RGB", (SIZE, SIZE))
    for y in range(SIZE):
        for x in range(SIZE):
            image.putpixel((x, y), noise(rng, base, spread, tint))

    for color, count in specks:
        for _ in range(count):
            image.putpixel((rng.randrange(SIZE), rng.randrange(SIZE)), noise(rng, color, 6))

    return image


def dirt():
    return speckled(11, (121, 85, 58), 12, [((92, 62, 41), 14), ((150, 110, 78), 10), ((128, 128, 124), 4)])


def grass_top():
    return speckled(23, (96, 158, 62), 14, [((70, 128, 44), 16), ((128, 186, 84), 12)])


def grass_side():
    """Tierra con una franja de hierba arriba, de borde irregular."""
    rng = random.Random(37)
    image = dirt()
    top = grass_top()
    depth = 3
    for x in range(SIZE):
        depth = max(2, min(5, depth + rng.choice((-1, 0, 0, 1))))
        for y in range(depth):
            image.putpixel((x, y), top.getpixel((x, y)))
        # El borde, algo más oscuro: da relieve
        r, g, b = image.getpixel((x, depth - 1))
        image.putpixel((x, depth - 1), (int(r * 0.82), int(g * 0.82), int(b * 0.82)))

    return image


def sand():
    return speckled(41, (226, 214, 160), 7, [((204, 190, 136), 12), ((240, 230, 184), 10)])


def stone():
    """Gris con vetas horizontales cortas."""
    rng = random.Random(53)
    image = speckled(53, (126, 126, 128), 6)
    for _ in range(9):
        x, y, length = rng.randrange(SIZE), rng.randrange(SIZE), rng.randint(2, 5)
        color = noise(rng, rng.choice(((104, 104, 106), (146, 146, 148))), 4)
        for i in range(length):
            image.putpixel(((x + i) % SIZE, y), color)

    return image


def cobblestone():
    """Piedras irregulares: cada píxel es de la piedra cuyo centro tiene más cerca; entre dos, junta oscura."""
    rng = random.Random(67)
    centers = [(rng.uniform(0, SIZE), rng.uniform(0, SIZE), rng.randint(104, 156)) for _ in range(9)]
    image = Image.new("RGB", (SIZE, SIZE))
    for y in range(SIZE):
        for x in range(SIZE):
            # Distancias dando la vuelta por los bordes, para que la textura case al repetirse
            ranked = sorted(
                (min(abs(x - cx), SIZE - abs(x - cx)) ** 2 + min(abs(y - cy), SIZE - abs(y - cy)) ** 2, shade) for cx, cy, shade in centers
            )
            if ranked[1][0] - ranked[0][0] < 5.0:
                shade = 74
            else:
                shade = ranked[0][1]
            image.putpixel((x, y), noise(rng, (shade, shade, shade + 2), 6))

    return image


def stonebrick():
    """Dos hiladas de ladrillos, la de abajo desplazada medio ladrillo, con junta oscura y un filo claro arriba."""
    rng = random.Random(79)
    image = Image.new("RGB", (SIZE, SIZE))
    for y in range(SIZE):
        row = y // 8
        for x in range(SIZE):
            in_row_y = y % 8
            in_brick_x = (x + (4 if row else 0)) % 16
            if in_row_y == 7 or in_brick_x == 15:
                color = (86, 88, 90)
            elif in_row_y == 0:
                color = (150, 152, 154)
            else:
                color = (124, 126, 128)
            image.putpixel((x, y), noise(rng, color, 5))

    return image


def diamond():
    """Bloque turquesa con marco y facetas en diagonal."""
    rng = random.Random(83)
    image = Image.new("RGB", (SIZE, SIZE))
    for y in range(SIZE):
        for x in range(SIZE):
            if x in (0, SIZE - 1) or y in (0, SIZE - 1):
                color = (38, 150, 160)
            elif x in (1, SIZE - 2) or y in (1, SIZE - 2):
                color = (170, 240, 236)
            else:
                band = (x + y) // 3 % 3
                color = ((88, 214, 208), (120, 232, 224), (64, 192, 196))[band]
            image.putpixel((x, y), noise(rng, color, 4))

    return image


def wool(seed=97):
    """Amarillo con tejido: hilos alternos algo más claros y más oscuros."""
    rng = random.Random(seed)
    image = Image.new("RGB", (SIZE, SIZE))
    for y in range(SIZE):
        for x in range(SIZE):
            weave = ((x // 2 + y // 2) % 2) * 10 - 5
            image.putpixel((x, y), noise(rng, (244 + weave // 2, 196 + weave, 52 + weave), 5))

    return image


def put(image, pixels, color):
    for x, y in pixels:
        image.putpixel((x, y), color)


WHITE = (250, 250, 250)
ORANGE = (236, 126, 40)
DARK = (58, 34, 20)


def patito_side():
    """El cuerpo del patito: amarillo, con la cabeza blanca asomando por arriba."""
    image = wool(101)
    for x in range(SIZE):
        for y in range(2 if x % 5 in (1, 2) else 1):
            image.putpixel((x, y), WHITE)
    return image


def patito_top():
    return Image.new("RGB", (SIZE, SIZE), WHITE)


def patito_bottom():
    image = wool(103)
    put(image, [(4, 0), (5, 0), (4, 1), (5, 1), (10, 0), (11, 0), (10, 1), (11, 1)], ORANGE)
    return image


def patito_front():
    """La cara: dos ojos, el pico y las patas."""
    image = patito_side()
    put(image, [(3, 6), (12, 6)], DARK)
    put(image, [(6, 9), (7, 9), (8, 9), (9, 9), (7, 10), (8, 10)], ORANGE)
    put(image, [(4, 14), (5, 14), (4, 15), (5, 15), (10, 14), (11, 14), (10, 15), (11, 15)], ORANGE)
    return image


TEXTURES = {
    "block-dirt.png": dirt,
    "block-grass_block-2.png": grass_top,
    "block-grass_block-0145.png": grass_side,
    "block-sand.png": sand,
    "block-stone.png": stone,
    "block-cobblestone.png": cobblestone,
    "block-stonebrick.png": stonebrick,
    "block-diamond_block.png": diamond,
    "block-yellow_wool.png": wool,
    "block-patito-015.png": patito_side,
    "block-patito-2.png": patito_top,
    "block-patito-3.png": patito_bottom,
    "block-patito-4.png": patito_front,
}


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)

    folder = sys.argv[1]
    os.makedirs(folder, exist_ok=True)
    for name, make in TEXTURES.items():
        make().save(os.path.join(folder, name))
        print(name)


if __name__ == "__main__":
    main()
