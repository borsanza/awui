#pragma once

#include "ArcadeContainer.h"

namespace awui {
	namespace Emulation::Chip8 {
		class CPU;
	}

	namespace Windows::Emulators {
		class Chip8 : public ArcadeContainer {
		  private:
			static bool s_invertedColors;
			bool m_lastInverted;

			Emulation::Chip8::CPU *m_cpu;
			Drawing::Image *m_image;

			int ConvertKeyAwToChip8(Forms::Input::Keys::Enum key);
			int ConvertRemoteKeyToChip8(Forms::Input::RemoteButtons::Enum button);
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

			virtual bool OnKeyPress(Forms::Input::Keys::Enum key);
			virtual bool OnKeyUp(Forms::Input::Keys::Enum key);
			bool OnRemoteKeyPress(int which, Forms::Input::RemoteButtons::Enum button);
			bool OnRemoteKeyUp(int which, Forms::Input::RemoteButtons::Enum button);
		};
	} // namespace Windows::Emulators
} // namespace awui
