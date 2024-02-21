#pragma once

#include "ArcadeContainer.h"

namespace awui {
	namespace Emulation::Chip8 {
		class CPU;
	}

	using namespace awui::Emulation::Chip8;

	namespace Windows::Emulators {
		class Chip8 : public ArcadeContainer {
			private:
				static bool m_invertedColors;
				bool m_lastInverted;

				CPU * m_cpu;
				Drawing::Image * m_image;

				int ConvertKeyAwToChip8(Keys::Enum key);
				int ConvertRemoteKeyToChip8(RemoteButtons::Enum button);
				void CheckBackcolor();
				void UpdateImage();

			public:
				Chip8();
				virtual ~Chip8();

				virtual bool IsClass(Classes objectClass) const override;

				virtual int GetType() const { return Types::Chip8;}

				void LoadRom(const String file);

				virtual void OnTick(float deltaTime);
				virtual void OnPaint(OpenGL::GL* gl);
				int GetChip8Mode() const;
				void SetInvertedColors(bool mode);

				virtual bool OnKeyPress(Keys::Enum key);
				virtual bool OnKeyUp(Keys::Enum key);
				bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
				bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);
		};
	}
}
