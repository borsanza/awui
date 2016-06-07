#pragma once

#include "ArcadeContainer.h"

#define TOTALSAVED 60

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Motherboard;
		}
	}

	using namespace awui::Emulation::MasterSystem;

	namespace Windows {
		namespace Emulators {
			class DebuggerSMS;

			class MasterSystem : public ArcadeContainer {
				private:
					Drawing::Image * _image;
					Motherboard * _cpu;
					DebuggerSMS * _debugger;
					bool _pause;
					bool _invertKeys;

					uint8_t GetPad(int which) const;

					int _first;
					int _last;
					int _actual;
					long long _lastTick;
					uint8_t * _savedData[TOTALSAVED];

				public:
					MasterSystem();
					virtual ~MasterSystem();

					virtual int IsClass(Classes::Enum objectClass) const;
					virtual int GetType() const { return Types::MasterSystem;}

					void LoadRom(const String file);

					virtual void OnTick();
					void RunOpcode();

					Motherboard * GetCPU();

					virtual void OnPaint(OpenGL::GL* gl);
					virtual bool OnKeyPress(Keys::Enum key);

					uint32_t GetCRC32();

					virtual void SetDebugger(DebuggerSMS * debugger) { this->_debugger = debugger; };
					inline void SetInvertKeys(bool mode) { this->_invertKeys = mode; }

					virtual bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
					virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);

					virtual void SetSoundEnabled(bool mode);
			};
		}
	}
}
