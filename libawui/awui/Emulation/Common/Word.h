#pragma once

#include <stdint.h>

namespace awui {
	namespace Emulation {
		union Word {
			uint16_t W;
			struct {
				uint8_t L;
				uint8_t H;
			};
		};
	}
}
