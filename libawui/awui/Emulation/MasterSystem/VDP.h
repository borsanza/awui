#ifndef _AWUI_EMULATION_MASTERSYSTEM_VDP_H
#define _AWUI_EMULATION_MASTERSYSTEM_VDP_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class VDP {
				private:
					uint16_t _width;
					uint16_t _height;

					uint16_t _line;
					uint16_t _col;
					uint8_t _status;

				public:
					VDP();
					~VDP();

					void OnTick();

					uint8_t GetStatus();
			};
		}
	}
}

#endif