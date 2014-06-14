#ifndef _AWUI_WINDOWS_EMULATORS_CHIP8_H
#define _AWUI_WINDOWS_EMULATORS_CHIP8_H

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Drawing {
		class Image;
	}

	namespace Emulation {
		namespace Chip8 {
			class CPU;
		}
	}

	namespace OpenGL {
		class GL;
	}

	using namespace awui::Emulation::Chip8;
	using namespace awui::Windows::Forms;

	namespace Windows {
		namespace Emulators {
			class Chip8 : public Control {
				private:
					CPU * _cpu;
					Drawing::Image * _image;

				public:
					Chip8();
					virtual ~Chip8();

					virtual int IsClass(Classes::Enum objectClass) const;

					void LoadRom(const String file);

					virtual void OnTick();
					virtual void OnPaint(OpenGL::GL* gl);
					int GetChip8Mode();
			};
		}
	}
}

#endif