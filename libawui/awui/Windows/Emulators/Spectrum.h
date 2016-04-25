#ifndef _AWUI_WINDOWS_EMULATORS_SPECTRUM_H
#define _AWUI_WINDOWS_EMULATORS_SPECTRUM_H

#include "ArcadeContainer.h"

#define TOTALSAVED 60

namespace awui {
	namespace Emulation {
		namespace Spectrum {
			class Motherboard;
		}
	}

	namespace Windows {
		namespace Emulators {
			class Spectrum : public ArcadeContainer {
				private:
					Drawing::Image * _image;
					awui::Emulation::Spectrum::Motherboard * _cpu;
					int _multiply;
					bool _canChangeControl;
					bool _pause;
					bool _invertKeys;
					int _fileSlot;

					uint8_t GetPad(int which) const;

					int _first;
					int _last;
					long long _lastTick;
					void CheckLimits();

					uint32_t _colors[16];

					void CallKey(int key, bool pressed);
					void DoKey(Keys::Enum key, bool pressed);

				public:
					Spectrum();
					virtual ~Spectrum();

					virtual int IsClass(Classes::Enum objectClass) const;
					virtual int GetType() const { return 3;}

					void LoadRom(const String file);

					virtual void OnTick();

					awui::Emulation::Spectrum::Motherboard * GetCPU();

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
