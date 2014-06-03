// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIPROCESSOR_H__
#define __AWUIPROCESSOR_H__

#include <awui/String.h>

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Screen;
			class Memory;
			class Registers;

			class Processor {
				private:
					int _pc;
					int _sp;
					Screen * _screen;
					Registers * _registers;
					Memory * _memory;

				public:
					Processor();
					~Processor();

					void LoadRom(const String file);
			};
		}
	}
}

#endif
