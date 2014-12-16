#ifndef _AWUI_WINDOWS_EMULATORS_SPECTRUM_H
#define _AWUI_WINDOWS_EMULATORS_SPECTRUM_H

#include <awui/Windows/Forms/Button.h>

#define TOTALSAVED 60

namespace awui {
	namespace Drawing {
		class Image;
	}

	namespace Emulation {
		namespace Spectrum {
			class CPU;
		}
	}

	namespace OpenGL {
		class GL;
	}

	using namespace awui::Emulation::Spectrum;
	using namespace awui::Windows::Forms;

	namespace Windows {
		namespace Emulators {
			class Spectrum : public Button {
				private:
					Drawing::Image * _image;
					CPU * _cpu;
					int _multiply;
					bool _canChangeControl;
					bool _pause;
					bool _invertKeys;

					uint8_t GetPad(int which) const;

					int _first;
					int _last;
					int _actual;
					long long _lastTick;
					uint8_t * _savedData[TOTALSAVED];
					void CheckLimits();

				public:
					Spectrum();
					virtual ~Spectrum();

					virtual int IsClass(Classes::Enum objectClass) const;

					void LoadRom(const String file);

					virtual void OnTick();
					void RunOpcode();

					CPU * GetCPU();

					virtual void OnPaint(OpenGL::GL* gl);
					virtual bool OnKeyPress(Keys::Enum key);

					void SetMultiply(int multiply);
					uint32_t GetCRC32();

					inline void SetInvertKeys(bool mode) { this->_invertKeys = mode; }

					bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
					bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);

					void SetSoundEnabled(bool mode);
			};
		}
	}
}

#endif
