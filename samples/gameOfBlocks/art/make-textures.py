#!/usr/bin/env python3
"""Genera las texturas de los bloques de gameOfBlocks (16x16, PNG).

Son propias: se dibujan aquí, píxel a píxel, con formas sencillas y algo de azar con semilla fija (siempre salen
iguales). Para cambiarlas, se toca este fichero y se vuelven a generar:

    samples/gameOfBlocks/art/make-textures.py build/samples/gameOfBlocks/images

Las que cubren el terreno casan consigo mismas al repetirse (todo se calcula dando la vuelta por los bordes).
Las de prueba (block-empty.png y block-pattern-*.png) no se generan aquí: están dibujadas a mano.
"""

import math
import os
import random
import sys

from PIL import Image

SIZE = 16


def rgb(text):
    return tuple(int(text[i : i + 2], 16) for i in (1, 3, 5))


def shade(color, amount):
    """Más claro (amount > 0) o más oscuro (< 0), en niveles."""
    return tuple(max(0, min(255, c + amount)) for c in color)


def mix(a, b, t):
    return tuple(int(round(x + (y - x) * t)) for x, y in zip(a, b))


def new(color=(0, 0, 0)):
    return Image.new("RGB", (SIZE, SIZE), color)


def put(image, x, y, color):
    """Dando la vuelta por los bordes."""
    image.putpixel((x % SIZE, y % SIZE), color)


def get(image, x, y):
    return image.getpixel((x % SIZE, y % SIZE))


# ---------------------------------------------------------------------------------------------------------------------
# Terreno


def grass_top():
    """Césped: manchas de dos verdes, briznas en diagonal y un par de flores."""
    rng = random.Random(7)
    dark, mid, light, tip = rgb("#3f7d3a"), rgb("#58a148"), rgb("#74bd5a"), rgb("#a4dd78")
    image = new()
    for y in range(SIZE):
        for x in range(SIZE):
            # Manchas suaves que casan al repetirse
            wave = math.sin((x + 2 * math.sin(y * math.pi / 8)) * math.pi / 4) + math.sin((y + x * 0.5) * math.pi / 4)
            base = mix(mid, light, 0.5 + 0.25 * wave)
            put(image, x, y, shade(base, rng.randint(-5, 5)))

    # Briznas: la base oscura y la punta clara, subiendo hacia la derecha
    for _ in range(11):
        x, y = rng.randrange(SIZE), rng.randrange(SIZE)
        put(image, x, y, dark)
        put(image, x + 1, y - 1, mix(mid, tip, 0.5))
        if rng.random() < 0.6:
            put(image, x + 1, y - 2, tip)

    # Flores: una blanca y una amarilla, de un píxel con su sombra
    for color, (x, y) in ((rgb("#f6f4e4"), (4, 11)), (rgb("#f3d44e"), (12, 4))):
        put(image, x, y, color)
        put(image, x, y + 1, dark)

    return image


def dirt():
    """Tierra: estratos horizontales ondulados, piedrecitas con su sombra y algún hueco."""
    rng = random.Random(11)
    dark, mid, light = rgb("#6b4630"), rgb("#85583b"), rgb("#9c6c49")
    image = new()
    for y in range(SIZE):
        for x in range(SIZE):
            band = math.sin((y + 1.5 * math.sin(x * math.pi / 8)) * math.pi / 4)
            base = mix(dark, light, 0.5 + 0.3 * band)
            put(image, x, y, shade(mix(base, mid, 0.4), rng.randint(-6, 6)))

    for x, y in ((2, 3), (10, 6), (6, 12), (13, 13)):
        put(image, x, y, rgb("#b9a58c"))
        put(image, x + 1, y, rgb("#a8937a"))
        put(image, x, y + 1, rgb("#4f3222"))
        put(image, x + 1, y + 1, rgb("#4f3222"))

    for _ in range(7):
        put(image, rng.randrange(SIZE), rng.randrange(SIZE), rgb("#523524"))

    return image


def grass_side():
    """El lado de un bloque de hierba: la tierra, con el césped colgando por arriba y alguna raíz."""
    rng = random.Random(13)
    image = dirt()
    top = grass_top()
    dark = rgb("#356b31")

    # El borde del césped sube y baja poco a poco, y casa al repetirse
    depth = [3 + round(1.4 * math.sin(x * math.pi / 8) + 0.9 * math.sin(x * math.pi / 4 + 1.0)) for x in range(SIZE)]
    for x in range(SIZE):
        for y in range(depth[x]):
            put(image, x, y, get(top, x, y + 5))

        put(image, x, depth[x] - 1, dark)                                # El borde, en sombra
        put(image, x, depth[x], shade(get(image, x, depth[x]), -22))     # Y la sombra que da a la tierra

    # Hebras que cuelgan y raíces finas
    for x in (1, 6, 11):
        put(image, x, depth[x], rgb("#4c9140"))
        put(image, x, depth[x] + 1, dark)

    for x in (4, 9, 14):
        for i in range(rng.randint(2, 3)):
            put(image, x, depth[x] + 1 + i, rgb("#5a3a27"))

    return image


def sand():
    """Arena: ondas de duna, con granos claros y oscuros."""
    rng = random.Random(17)
    low, base, high = rgb("#d9c68d"), rgb("#e9d9a6"), rgb("#f5eac2")
    image = new()
    for y in range(SIZE):
        for x in range(SIZE):
            ripple = math.sin((y + 1.6 * math.sin(x * math.pi / 8)) * 3 * math.pi / 8)
            color = mix(base, high, ripple) if ripple > 0 else mix(base, low, -ripple * 0.8)
            put(image, x, y, shade(color, rng.randint(-3, 3)))

    for _ in range(6):
        put(image, rng.randrange(SIZE), rng.randrange(SIZE), rgb("#c9a87a"))
    for _ in range(4):
        put(image, rng.randrange(SIZE), rng.randrange(SIZE), rgb("#fdf7e2"))

    return image


def stone():
    """Roca: gris azulado con vetas, y grietas con su filo claro debajo."""
    rng = random.Random(19)
    dark, base, light, crack = rgb("#596068"), rgb("#707881"), rgb("#8a929b"), rgb("#444a51")
    image = new()
    for y in range(SIZE):
        for x in range(SIZE):
            vein = math.sin((x + y * 0.5) * math.pi / 8) + 0.6 * math.sin((x - y) * math.pi / 4)
            put(image, x, y, shade(mix(base, light if vein > 0 else dark, abs(vein) * 0.35), rng.randint(-4, 4)))

    for x, y, steps in ((1, 3, 7), (9, 10, 6)):
        for _ in range(steps):
            put(image, x, y, crack)
            put(image, x, y + 1, light)
            x += 1
            y += rng.choice((0, 0, 1))

    return image


def cobblestone():
    """Empedrado: cantos redondeados, con luz arriba a la izquierda y sombra abajo a la derecha, y algo de musgo."""
    rng = random.Random(23)
    tones = (rgb("#737b84"), rgb("#868e97"), rgb("#99a1a9"))
    mortar, moss = rgb("#4a5057"), rgb("#5f8a4c")
    # Los centros, repartidos en una rejilla de 4x3 y movidos un poco: piedras de tamaño parecido, sin huecos
    centers = [
        ((column + 0.5) * SIZE / 4 + rng.uniform(-1.2, 1.2) + (2 if row % 2 else 0), (row + 0.5) * SIZE / 3 + rng.uniform(-1.0, 1.0), rng.choice(tones), rng.random() < 0.2)
        for row in range(3)
        for column in range(4)
    ]

    def wrapped(a, b):
        delta = a - b
        return delta - SIZE * round(delta / SIZE)

    image = new()
    for y in range(SIZE):
        for x in range(SIZE):
            ranked = sorted((wrapped(x, cx) ** 2 + wrapped(y, cy) ** 2, i) for i, (cx, cy, _, _) in enumerate(centers))
            cx, cy, tone, mossy = centers[ranked[0][1]]
            if math.sqrt(ranked[1][0]) - math.sqrt(ranked[0][0]) < 0.75:
                put(image, x, y, shade(mortar, rng.randint(-3, 3)))
                continue

            # La luz viene de arriba a la izquierda
            lit = -(wrapped(x, cx) + wrapped(y, cy)) * 5
            color = shade(tone, int(max(-18, min(18, lit))) + rng.randint(-3, 3))
            if mossy and wrapped(y, cy) > 0:
                color = mix(color, moss, 0.55)
            put(image, x, y, color)

    return image


def stonebrick():
    """Sillares: dos hiladas a matajunta, cada piedra biselada (luz arriba y a la izquierda), una con una grieta."""
    rng = random.Random(29)
    face, light, dark, mortar = rgb("#8a9199"), rgb("#aab1b8"), rgb("#5f666e"), rgb("#454a50")
    image = new()
    for y in range(SIZE):
        row = y // 8
        for x in range(SIZE):
            by = y % 8
            bx = (x + 8 * row) % SIZE
            if by == 7 or bx == 15:
                color = mortar
            elif by == 0 or bx == 0:
                color = light
            elif by == 6 or bx == 14:
                color = dark
            else:
                color = shade(face, int(3 * math.sin((x * 3 + y * 5) * 0.9)))
            put(image, x, y, shade(color, rng.randint(-3, 3)))

    # La grieta, en la piedra de arriba, y musgo en una junta
    for x, y in ((5, 1), (5, 2), (6, 3), (6, 4), (7, 5)):
        put(image, x, y, mortar)
        put(image, x + 1, y, light)
    for x in (11, 12, 13):
        put(image, x, 7, rgb("#5f8a4c"))

    return image


# ---------------------------------------------------------------------------------------------------------------------
# Bloques sueltos


def diamond():
    """Diamante tallado: facetas que salen del centro, cada una con su brillo, dentro de un marco, y dos destellos."""
    frame, deep, base, light, glint = rgb("#0f5f6c"), rgb("#1b8fa0"), rgb("#2fbccb"), rgb("#86ecf0"), rgb("#e6ffff")
    tones = (light, base, deep, base, mix(base, light, 0.5), deep, base, mix(base, deep, 0.5))
    image = new()
    for y in range(SIZE):
        for x in range(SIZE):
            if x in (0, SIZE - 1) or y in (0, SIZE - 1):
                put(image, x, y, frame)
                continue

            dx, dy = x - 7.5, y - 7.5
            sector = int(((math.atan2(dy, dx) + math.pi) / (2 * math.pi)) * 8) % 8
            color = tones[sector]
            # La tabla (el centro), más clara, y un filo oscuro entre la tabla y la corona
            distance = max(abs(dx), abs(dy))
            if distance < 2.6:
                color = mix(color, glint, 0.55)
            elif distance < 3.6:
                color = mix(color, frame, 0.35)
            put(image, x, y, color)

    for x, y in ((4, 3), (11, 12)):
        for ox, oy in ((0, 0), (1, 0), (-1, 0), (0, 1), (0, -1)):
            put(image, x + ox, y + oy, glint)

    return image


def wool():
    """Lana de punto: hileras de puntos en uve, con la sombra debajo."""
    rng = random.Random(31)
    dark, base, light = rgb("#d6a11c"), rgb("#f0bf2e"), rgb("#f8d85e")
    image = new(base)
    for y in range(SIZE):
        for x in range(SIZE):
            cx, cy = x % 4, y % 4
            if (cx, cy) in ((0, 0), (3, 0), (1, 1), (2, 1)):
                color = light
            elif (cx, cy) in ((1, 2), (2, 2), (0, 1), (3, 1)):
                color = dark
            else:
                color = base
            put(image, x, y, shade(color, rng.randint(-3, 3)))

    return image


# ---------------------------------------------------------------------------------------------------------------------
# El patito


def stamp(image, rows, colors, left=0, top=0):
    """Dibuja un patrón de letras: cada letra es un color de colors; el punto no pinta."""
    for y, row in enumerate(rows):
        for x, letter in enumerate(row):
            if letter != ".":
                image.putpixel((left + x, top + y), colors[letter])


DUCK, DUCK_DARK, DUCK_LIGHT = rgb("#f6cf3c"), rgb("#deab25"), rgb("#fbe27a")
BEAK, BEAK_DARK, EYE, BLUSH, WHITE = rgb("#f08a24"), rgb("#c96a14"), rgb("#2b1d14"), rgb("#f4a37a"), rgb("#fffdf4")


def plumage(seed):
    """El amarillo del patito: más claro arriba, con plumitas sueltas."""
    rng = random.Random(seed)
    image = new()
    for y in range(SIZE):
        for x in range(SIZE):
            put(image, x, y, shade(mix(DUCK_LIGHT, DUCK, min(1.0, y / 9)), rng.randint(-3, 3)))

    for _ in range(9):
        x, y = rng.randrange(1, SIZE - 1), rng.randrange(3, SIZE - 1)
        put(image, x, y, DUCK_DARK)
        put(image, x + 1, y - 1, DUCK_LIGHT)

    return image


def feet(image, y):
    for x in (3, 4, 5, 10, 11, 12):
        image.putpixel((x, y), BEAK)
        image.putpixel((x, y + 1), BEAK_DARK if x in (3, 5, 10, 12) else BEAK)


def patito_front():
    """La cara: ojos con su brillo, pico, mofletes y las patas asomando."""
    image = plumage(41)
    for x in (4, 10):
        for ox, oy in ((0, 0), (1, 0), (0, 1), (1, 1)):
            image.putpixel((x + ox, 5 + oy), EYE)
        image.putpixel((x, 5), WHITE)

    for x in range(6, 10):
        image.putpixel((x, 8), BEAK)
        image.putpixel((x, 9), BEAK if x in (6, 9) else BEAK_DARK)
    image.putpixel((7, 10), BEAK)
    image.putpixel((8, 10), BEAK)

    for x in (3, 4, 11, 12):
        image.putpixel((x, 8), BLUSH)

    feet(image, 14)
    return image


def patito_side():
    """El costado (y la espalda): un ala recogida, con las puntas de las plumas por abajo."""
    image = plumage(43)
    stamp(
        image,
        (
            "..dddddd....",
            ".dlllllldd..",
            "dllllllllld.",
            "dlllllllllld",
            "dllllllllld.",
            "dldlldlldd..",
            ".d.dd.dd....",
        ),
        {"d": DUCK_DARK, "l": DUCK_LIGHT},
        left=2,
        top=5,
    )
    return image


def patito_top():
    """La cabeza desde arriba: un mechón de tres plumas."""
    image = plumage(47)
    stamp(
        image,
        (
            "..d...d..",
            ".dld.dld.",
            ".dld.dld.",
            "..dldld..",
            "..dllld..",
            "...dld...",
            "....d....",
        ),
        {"d": DUCK_DARK, "l": WHITE},
        left=4,
        top=4,
    )
    return image


def patito_bottom():
    """Por debajo: la tripa clara y las dos patas."""
    image = plumage(53)
    for y in range(4, 12):
        for x in range(4, 12):
            if (x - 7.5) ** 2 + (y - 7.5) ** 2 < 15:
                image.putpixel((x, y), mix(image.getpixel((x, y)), WHITE, 0.6))
    feet(image, 0)
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
