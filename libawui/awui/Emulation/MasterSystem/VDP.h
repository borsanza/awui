#ifndef _AWUI_EMULATION_MASTERSYSTEM_VDP_H
#define _AWUI_EMULATION_MASTERSYSTEM_VDP_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Ram;

			class VDP {
				private:
					uint16_t _width;
					uint16_t _height;

					uint16_t _line;
					uint16_t _col;
					uint8_t _status;

					// 32 Color Ram
					uint8_t _cram[32];

					// 11 Registers
					uint8_t _registers[11];

					Ram * _vram;

				public:
					VDP();
					~VDP();

					bool OnTick();

					uint8_t GetStatus();
			};
		}
	}
}

#endif