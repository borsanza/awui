#ifndef _AWUI_EMULATION_CHIP8_SCREEN_H
#define _AWUI_EMULATION_CHIP8_SCREEN_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Screen {
				private:
					uint16_t _width;
					uint16_t _height;
					uint32_t * _data;

				public:
					Screen(uint16_t width, uint16_t height);
					virtual ~Screen();

					void Clear();

					bool SetPixelXOR(uint16_t x, uint16_t y, bool value);
					void SetPixel(uint16_t x, uint16_t y, uint32_t value);
					uint32_t GetPixel(uint16_t x, uint16_t y);

					uint16_t GetWidth();
					uint16_t GetHeight();

					void ScrollLeft(uint8_t columns);
					void ScrollRight(uint8_t columns);
					void ScrollUp(uint8_t lines);
					void ScrollDown(uint8_t lines);
			};
		}
	}
}

#endif
