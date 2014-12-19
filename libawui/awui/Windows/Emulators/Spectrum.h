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
			class Motherboard;
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
					Motherboard * _cpu;
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

					uint32_t _colors[16];

				public:
					Spectrum();
					virtual ~Spectrum();

					virtual int IsClass(Classes::Enum objectClass) const;

					void LoadRom(const String file);

					virtual void OnTick();

					Motherboard * GetCPU();

					virtual void OnPaint(OpenGL::GL* gl);

					void SetMultiply(int multiply);

					virtual bool OnKeyPress(Keys::Enum key);
					virtual bool OnKeyUp(Keys::Enum key);
					virtual bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
					virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);
			};
		}
	}
}

#endif
