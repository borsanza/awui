// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIPROCESSOR_H__
#define __AWUIPROCESSOR_H__

#include <awui/String.h>

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Graphics;
			class Registers;

			class Processor {
				private:
					int _pc;
					int _sp;
					Graphics * _graphics;
					Registers * _registers;

				public:
					Processor();
					~Processor();

					void LoadRom(const String file);
			};
		}
	}
}

#endif
