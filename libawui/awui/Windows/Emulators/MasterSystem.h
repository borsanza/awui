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
			class DebuggerSMS;

			class MasterSystem : public Button {
				private:
					Drawing::Image * _image;
					CPU * _cpu;
					DebuggerSMS * _debugger;
					int _multiply;
					bool _canChangeControl;

				public:
					MasterSystem();
					virtual ~MasterSystem();

					virtual int IsClass(Classes::Enum objectClass) const;

					void LoadRom(const String file);

					virtual void OnTick();
					void RunOpcode();

					CPU * GetCPU();

					virtual void OnPaint(OpenGL::GL* gl);
					virtual bool OnKeyPress(Keys::Enum key);

					void SetMultiply(int multiply);
					uint32_t GetCRC32();

					void SetDebugger(DebuggerSMS * debugger) { this->_debugger = debugger; };

					bool OnRemoteKeyPress(RemoteButtons::Enum button);
					bool OnRemoteKeyUp(RemoteButtons::Enum button);
			};
		}
	}
}

#endif
