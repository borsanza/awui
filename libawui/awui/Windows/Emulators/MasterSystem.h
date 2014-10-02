#ifndef _AWUI_WINDOWS_EMULATORS_MASTERSYSTEM_H
#define _AWUI_WINDOWS_EMULATORS_MASTERSYSTEM_H

#include <awui/Windows/Forms/Button.h>

namespace awui {
	namespace Drawing {
		class Image;
	}

	namespace Emulation {
		namespace MasterSystem {
			class CPU;
		}
	}

	namespace OpenGL {
		class GL;
	}

	using namespace awui::Emulation::MasterSystem;
	using namespace awui::Windows::Forms;

	namespace Windows {
		namespace Emulators {
			class MasterSystem : public Button {
				private:
					Drawing::Image * _image;
					CPU * _cpu;

				public:
					MasterSystem();
					virtual ~MasterSystem();

					virtual int IsClass(Classes::Enum objectClass) const;

					void LoadRom(const String file);

					virtual void OnTick();

					CPU * GetCPU();

					virtual void OnPaint(OpenGL::GL* gl);
			};
		}
	}
}

#endif