#ifndef _AWUI_EMULATION_CHIP8_INPUT_H
#define _AWUI_EMULATION_CHIP8_INPUT_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Input {
				private:

				public:
					Input();
					~Input();

					bool IsKeyPressed(uint8_t key);
			};
		}
	}
}

#endif