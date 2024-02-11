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
					Drawing::Image * m_image;
					Motherboard * m_cpu;
					DebuggerSMS * m_debugger;
					bool m_pause;

					uint8_t m_buttonsPad1;
					uint8_t m_buttonsPad2;

					int m_first;
					int m_last;
					int m_actual;
					long long m_lastTick;
					uint8_t * m_savedData[TOTALSAVED];

				public:
					MasterSystem();
					virtual ~MasterSystem();

					virtual bool IsClass(Classes objectClass) const override;
					virtual int GetType() const { return Types::MasterSystem;}

					void LoadRom(const String file);

					virtual void OnTick();
					void RunOpcode();

					Motherboard * GetCPU();

					virtual void OnPaint(OpenGL::GL* gl);
					virtual bool OnKeyPress(Keys::Enum key);
					virtual bool OnKeyUp(Keys::Enum key);

					uint32_t GetCRC32();

					virtual void SetDebugger(DebuggerSMS * debugger) { m_debugger = debugger; };

					virtual void SetSoundEnabled(bool mode);
			};
		}
	}
}
