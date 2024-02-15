#pragma once

#include "ArcadeContainer.h"

#define TOTALSAVED 60

namespace awui {
	namespace Emulation::MasterSystem {
		class Motherboard;
	}

	using namespace awui::Emulation::MasterSystem;

	namespace Windows::Emulators {
		class DebuggerSMS;

		class MasterSystem : public ArcadeContainer {
			private:
				Drawing::Image * m_image;
				Motherboard * m_cpu;
				DebuggerSMS * m_debugger;
				bool m_pause;

				uint8_t m_keys1;
				uint8_t m_keys2;
				uint8_t m_joys1;
				uint8_t m_joys2;
				uint8_t m_axis1;
				uint8_t m_axis2;
				bool m_invertButtons;

				int m_first;
				int m_last;
				int m_actual;
				long long m_lastTick;
				uint8_t * m_savedData[TOTALSAVED];

				void RefreshPads();

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
				bool RefreshButtons(JoystickButtonEventArgs* e);
				virtual bool OnJoystickButtonDown(JoystickButtonEventArgs* e);
				virtual bool OnJoystickButtonUp(JoystickButtonEventArgs* e);
				virtual bool OnJoystickAxisMotion(JoystickAxisMotionEventArgs* e);

				void TimeReverse();
				void TimeForward();
				void Pause(bool mode);

				uint32_t GetCRC32();

				virtual void SetDebugger(DebuggerSMS * debugger) { m_debugger = debugger; };

				virtual void SetSoundEnabled(bool mode);
		};
	}
}
