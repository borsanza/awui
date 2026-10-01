#pragma once

#include "ArcadeContainer.h"

namespace awui {
	namespace Emulation::Chip8 {
		class CPU;
	}

	namespace UI::Emulators {
		class Chip8 : public ArcadeContainer {
		  private:
			static bool s_invertedColors;
			bool m_lastInverted;

			Emulation::Chip8::CPU *m_cpu;
			Drawing::Image *m_image;

			int ConvertKeyAwToChip8(UI::Input::Keys::Enum key);
			int ConvertRemoteKeyToChip8(UI::Input::RemoteButtons::Enum button);
			void CheckBackcolor();
			void UpdateImage();

		  public:
			Chip8();
			virtual ~Chip8();

			virtual int GetType() const { return Types::Chip8; }

			void LoadRom(const String file);

			virtual void OnTick(float deltaSeconds);
			virtual void OnPaint(OpenGL::GL *gl);
			int GetChip8Mode() const;
			void SetInvertedColors(bool mode);

			// El zumbador suena solo si es el emulador que está en juego
			virtual void SetSoundEnabled(bool mode) override;

			virtual bool OnKeyPress(UI::Input::Keys::Enum key);
			virtual bool OnKeyUp(UI::Input::Keys::Enum key);
			bool OnRemoteKeyPress(int which, UI::Input::RemoteButtons::Enum button);
			bool OnRemoteKeyUp(int which, UI::Input::RemoteButtons::Enum button);
		};
	} // namespace UI::Emulators
} // namespace awui
