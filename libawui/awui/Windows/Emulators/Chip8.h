// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUICHIP8_H__
#define __AWUICHIP8_H__

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Processor;
		}
	}

	using namespace awui::Emulation::Chip8;
	using namespace awui::Windows::Forms;

	namespace Windows {
		namespace Emulators {
			class Chip8 : public Control {
				private:
					Processor * _processor;

				public:
					Chip8();
					virtual ~Chip8();

					virtual int IsClass(Classes::Enum objectClass) const;

					void LoadRom(const String file);
			};
		}
	}
}

#endif
