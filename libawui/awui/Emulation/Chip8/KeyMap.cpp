/*
 * awui/Emulation/Chip8/KeyMap.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "KeyMap.h"

#include <set>

using namespace awui::Emulation::Chip8;

namespace {
	// Teclado hexadecimal del COSMAC VIP, por filas
	constexpr int Layout[4][4] = {
		{0x1, 0x2, 0x3, 0xC},
		{0x4, 0x5, 0x6, 0xD},
		{0x7, 0x8, 0x9, 0xE},
		{0xA, 0x0, 0xB, 0xF},
	};

	int Row(int key) {
		for (int r = 0; r < 4; r++)
			for (int c = 0; c < 4; c++)
				if (Layout[r][c] == key)
					return r;
		return 0;
	}

	int Col(int key) {
		for (int r = 0; r < 4; r++)
			for (int c = 0; c < 4; c++)
				if (Layout[r][c] == key)
					return c;
		return 0;
	}

	uint16_t Word(const std::vector<uint8_t> &rom, size_t i) {
		return (rom[i] << 8) | rom[i + 1];
	}

	// Teclas que comprueba la ROM con EX9E/EXA1. La tecla suele cargarse con un LD VX, NN poco antes; si no, se busca
	// en toda la ROM y vale si VX solo se carga con un valor (Tetris guarda las teclas en registros al empezar)
	uint16_t UsedKeys(const std::vector<uint8_t> &rom) {
		uint16_t keys = 0;
		size_t size = rom.size();
		for (size_t i = 0; i + 1 < size; i++) {
			uint16_t op = Word(rom, i);
			if (((op & 0xF0FF) != 0xE09E) && ((op & 0xF0FF) != 0xE0A1))
				continue;

			int x = (op >> 8) & 0xF;
			int key = -1;
			for (size_t back = 1; (back <= 8) && (back * 2 <= i); back++) {
				uint16_t o = Word(rom, i - back * 2);
				int kind = o >> 12;
				int ox = (o >> 8) & 0xF;
				if ((o & 0xFF00) == (0x6000 | (x << 8))) {
					key = o & 0xFF;
					break;
				}

				// Algo cambia VX, o hay un salto: el LD de antes ya no vale
				if (((kind == 0x7 || kind == 0x8 || kind == 0xC) && (ox == x)) || ((o & 0xF0FF) == 0xF065 && x <= ox) ||
					kind == 0x1 || kind == 0x2)
					break;
			}

			if (key < 0) {
				std::set<int> values;
				for (size_t j = i % 2; j + 1 < size; j += 2)
					if (rom[j] == (0x60 | x))
						values.insert(rom[j + 1]);
				if (values.size() == 1)
					key = *values.begin();
			}

			if ((key >= 0) && (key < 16))
				keys |= 1 << key;
		}

		return keys;
	}

	bool Has(uint16_t keys, int key) {
		return (keys >> key) & 1;
	}

	int Count(uint16_t keys) {
		int n = 0;
		for (int k = 0; k < 16; k++)
			n += Has(keys, k);
		return n;
	}
} // namespace

KeyMap KeyMap::Detect(const std::vector<uint8_t> &rom) {
	KeyMap map;
	uint16_t keys = UsedKeys(rom);
	if (!keys)
		return map;

	// Las dos convenciones conocidas: vale si usa al menos 3 de sus flechas, o 2 que formen un eje (izquierda y
	// derecha, o arriba y abajo). Se queda la que más coincide
	KeyMap octo;
	octo.up = 5;
	octo.left = 7;
	octo.right = 9;
	octo.down = 8;
	octo.ok = 6;
	const KeyMap *best = nullptr;
	int bestCount = 0;
	for (const KeyMap *l : {&map, &octo}) {
		int count = Has(keys, l->up) + Has(keys, l->down) + Has(keys, l->left) + Has(keys, l->right);
		bool axis = (Has(keys, l->left) && Has(keys, l->right)) || (Has(keys, l->up) && Has(keys, l->down));
		if (((count >= 3) || (axis && count >= 2)) && (count > bestCount)) {
			best = l;
			bestCount = count;
		}
	}

	if (best) {
		KeyMap result = *best;
		// Si no usa su tecla de acción, OK va a la primera tecla que no sea una flecha
		if (!Has(keys, result.ok)) {
			uint16_t rest = keys & ~((1 << result.up) | (1 << result.down) | (1 << result.left) | (1 << result.right));
			for (int r = 0; r < 4 && !Has(rest, result.ok); r++)
				for (int c = 0; c < 4; c++)
					if (Has(rest, Layout[r][c])) {
						result.ok = Layout[r][c];
						break;
					}
		}
		return result;
	}

	// Si no, por posición: dos teclas seguidas en una columna son arriba y abajo (la columna de más a la izquierda: en
	// Pong, el jugador 1), y dos seguidas en una fila, izquierda y derecha
	uint16_t free = keys;
	bool vertical = false;
	for (int c = 0; c < 4 && !vertical; c++)
		for (int r = 0; r < 3; r++)
			if (Has(free, Layout[r][c]) && Has(free, Layout[r + 1][c])) {
				map.up = Layout[r][c];
				map.down = Layout[r + 1][c];
				free &= ~((1 << map.up) | (1 << map.down));
				vertical = true;
				break;
			}

	bool horizontal = false;
	for (int r = 0; r < 4 && !horizontal; r++)
		for (int c = 0; c < 3; c++)
			if (Has(free, Layout[r][c]) && Has(free, Layout[r][c + 1])) {
				map.left = Layout[r][c];
				map.right = Layout[r][c + 1];
				free &= ~((1 << map.left) | (1 << map.right));
				horizontal = true;
				break;
			}

	// Sin pares: la de más a la izquierda y la de más a la derecha (Wipeout: A y D)
	if (!vertical && !horizontal && Count(keys) >= 2) {
		int left = -1;
		int right = -1;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				if (Has(free, Layout[r][c])) {
					if (left < 0)
						left = Layout[r][c];
					right = Layout[r][c];
				}
		map.left = left;
		map.right = right;
		free &= ~((1 << left) | (1 << right));
	}

	// OK: la primera tecla que queda, si no forma pareja con otra (las del jugador 2 en Pong)
	for (int r = 0; r < 4; r++)
		for (int c = 0; c < 4; c++) {
			int key = Layout[r][c];
			if (!Has(free, key))
				continue;
			bool paired = (r > 0 && Has(free, Layout[r - 1][c])) || (r < 3 && Has(free, Layout[r + 1][c])) ||
						  (c > 0 && Has(free, Layout[r][c - 1])) || (c < 3 && Has(free, Layout[r][c + 1]));
			if (!paired) {
				map.ok = key;
				return map;
			}
		}

	return map;
}
