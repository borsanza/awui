#ifndef _AWUI_EMULATION_COMMON_WORD_H
#define _AWUI_EMULATION_COMMON_WORD_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		union Word {
			uint16_t W;
			struct {
				uint8_t L, H;
			};
		};
	}
}

#endif
