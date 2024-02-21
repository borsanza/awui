#pragma once

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
					void DoRemoteKey(RemoteButtons::Enum button, bool pressed);

					void SaveState();
					void LoadState();

				public:
					Spectrum();
					virtual ~Spectrum();

					virtual bool IsClass(Classes objectClass) const override;
					virtual int GetType() const { return Types::Spectrum;}

					void LoadRom(const String file);

					virtual void OnTick(float deltaTime);

					awui::Emulation::Spectrum::Motherboard * GetCPU();

					virtual void OnPaint(OpenGL::GL* gl);

					virtual bool OnKeyPress(Keys::Enum key);
					virtual bool OnKeyUp(Keys::Enum key);
					virtual bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
					virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);
					virtual void SetSoundEnabled(bool mode);

					awui::Emulation::Spectrum::TapeCorder * GetTapeCorder() { return this->_tapecorder; }
					awui::Emulation::Spectrum::Motherboard * GetMotherboard() { return this->_motherboard; }
			};
		}
	}
}
