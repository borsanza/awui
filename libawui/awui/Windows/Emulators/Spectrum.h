#ifndef _AWUI_WINDOWS_EMULATORS_SPECTRUM_H
#define _AWUI_WINDOWS_EMULATORS_SPECTRUM_H

#include "ArcadeContainer.h"

#define TOTALSAVED 60

namespace awui {
	namespace Emulation {
		namespace Spectrum {
			class Motherboard;
			class TapeCorder;
		}
	}

	namespace Windows {
		namespace Emulators {
			class Spectrum : public ArcadeContainer {
				private:
					awui::Emulation::Spectrum::Motherboard * _motherboard;
					int _multiply;
					bool _canChangeControl;
					bool _pause;
					int _fileSlot;

					awui::Emulation::Spectrum::TapeCorder * _tapecorder;

					uint8_t GetPad() const;

					int _first;
					int _last;
					long long _lastTick;
					void CheckLimits();

					void CallKey(int key, bool pressed);
					void DoKey(Keys::Enum key, bool pressed);
					void SaveState();
					void LoadState();
					void LoadCassette();

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
					virtual void SetSoundEnabled(bool mode);

					awui::Emulation::Spectrum::TapeCorder * GetTapeCorder() { return this->_tapecorder; }
			};
		}
	}
}

#endif
