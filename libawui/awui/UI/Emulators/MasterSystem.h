#pragma once

#include "ArcadeContainer.h"

#include <vector>

namespace awui {
	namespace Emulation::MasterSystem {
		class Motherboard;
	}

	namespace Emulation::Common {
		class RewindBuffer;
	}

	namespace UI::Emulators {
		class DebuggerSMS;

		class MasterSystem : public ArcadeContainer {
		  private:
			Drawing::Image *m_image;
			Emulation::MasterSystem::Motherboard *m_cpu;
			DebuggerSMS *m_debugger;
			bool m_pause;

			uint8_t m_keys1;
			uint8_t m_keys2;
			uint8_t m_joys1;
			uint8_t m_joys2;
			uint8_t m_axis1;
			uint8_t m_axis2;
			bool m_invertButtons;

			// Rebobinado: un estado por tick. Mientras se mantiene el botón se retrocede (o avanza) un frame por tick
			Emulation::Common::RewindBuffer *m_rewind;
			std::vector<uint8_t> m_state;
			bool m_rewinding;

			void ToggleSoundChannel(int channel);
			bool m_forwarding;

			void RefreshPads();

		  public:
			MasterSystem();
			virtual ~MasterSystem();

			virtual int GetType() const { return Types::MasterSystem; }

			void LoadRom(const String file);

			virtual void OnTick(float deltaSeconds);
			void RunOpcode();

			Emulation::MasterSystem::Motherboard *GetCPU();

			virtual void OnPaint(OpenGL::GL *gl);
			virtual bool OnKeyPress(UI::Input::Keys::Enum key);
			virtual bool OnKeyUp(UI::Input::Keys::Enum key);
			bool RefreshButtons(UI::Events::JoystickButtonEventArgs *e);
			virtual bool OnJoystickButtonDown(UI::Events::JoystickButtonEventArgs *e);
			virtual bool OnJoystickButtonUp(UI::Events::JoystickButtonEventArgs *e);
			virtual bool OnJoystickAxisMotion(UI::Events::JoystickAxisMotionEventArgs *e);

			virtual bool SaveAutoState() override;
			virtual bool LoadAutoState() override;

			void SetRewinding(bool mode);
			void SetForwarding(bool mode);
			void Pause(bool mode);

			uint32_t GetCRC32();

			virtual void SetDebugger(DebuggerSMS *debugger) { m_debugger = debugger; };

			virtual void SetSoundEnabled(bool mode);
		};
	} // namespace UI::Emulators
} // namespace awui
