// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIMEMORY_H__
#define __AWUIMEMORY_H__

#include <awui/String.h>

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Memory {
				private:
					unsigned char * _data;

				public:
					Memory(unsigned int size);
					~Memory();

					void LoadRom(const String file);
			};
		}
	}
}

#endif
