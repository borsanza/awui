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
			class Mainboard;
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
					Mainboard * _cpu;
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

					Mainboard * GetCPU();

					virtual void OnPaint(OpenGL::GL* gl);

					void SetMultiply(int multiply);
			};
		}
	}
}

#endif
