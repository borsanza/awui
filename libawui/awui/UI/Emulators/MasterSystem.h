#pragma once

#include "ArcadeContainer.h"

#include <vector>

namespace awui {
	namespace Emulation::MasterSystem {
		class Motherboard;
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

			void ToggleSoundChannel(int channel);

			void RefreshPads();

		  protected:
			virtual int GetStateSize() const override;
			virtual void SaveStateData(uint8_t *data) override;
			virtual void LoadStateData(uint8_t *data) override;
			virtual void ResetMachine() override;
			virtual KeyHelp::Section GetSystemKeys() const override;
			virtual void EmulateTime(float seconds) override;
			virtual void RewindFrame(uint8_t *data) override;
			virtual float GetFrameSeconds() const override;
			virtual void SetSoundMode(bool reverse, bool fastForward) override;

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


			virtual void SetRewinding(bool mode) override;
			virtual void SetForwarding(bool mode) override;
			void Pause(bool mode);

			uint32_t GetCRC32();

			virtual void SetDebugger(DebuggerSMS *debugger) { m_debugger = debugger; };

			virtual void SetSoundEnabled(bool mode);
		};
	} // namespace UI::Emulators
} // namespace awui
