#pragma once

#include <stdint.h>

namespace awui::Emulation::Chip8 {
	class Input {
	  private:
		bool m_keys[16];
		int m_lastKey;

	  public:
		Input();
		virtual ~Input();

		bool IsKeyPressed(uint8_t key);
		// Última tecla pulsada, y la olvida (-1 si no hay ninguna): cada pulsación se recoge una sola vez
		int TakeLastKey();

		void KeyDown(uint8_t key);
		void KeyUp(uint8_t key);
	};
} // namespace awui::Emulation::Chip8
