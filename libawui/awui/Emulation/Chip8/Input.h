#pragma once

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Input {
				private:
					bool _keys[16];
					int _lastKey;

				public:
					Input();
					virtual ~Input();

					bool IsKeyPressed(uint8_t key);
					int GetKey();

					void KeyDown(uint8_t key);
					void KeyUp(uint8_t key);
			};
		}
	}
}
