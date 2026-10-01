#pragma once

#include <cstdint>
#include <vector>

namespace awui::Emulation::Chip8 {
	// Teclas del teclado hexadecimal a las que van las flechas y el botón OK del mando. Cada juego usa las suyas: lo
	// habitual es 2/4/6/8 y 5, los de Octo usan 5/7/8/9 y 6 (WASD y E), y otros pares como 1/4 (Pong) o A/D (Wipeout)
	struct KeyMap {
		int up = 2;
		int down = 8;
		int left = 4;
		int right = 6;
		int ok = 5;

		// Mira qué teclas comprueba la ROM (EX9E/EXA1 con el LD VX, NN que carga la tecla) y las reparte según su
		// posición en el teclado. Si no se reconoce ninguna, se queda la asignación habitual
		static KeyMap Detect(const std::vector<uint8_t> &rom);
	};
} // namespace awui::Emulation::Chip8
