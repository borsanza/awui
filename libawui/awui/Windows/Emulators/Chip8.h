#pragma once

#include "ArcadeContainer.h"

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class CPU;
		}
	}

	using namespace awui::Emulation::Chip8;

	namespace Windows {
		namespace Emulators {
			class Chip8 : public ArcadeContainer {
				private:
					static bool _invertedColors;
					bool _lastInverted;

					CPU * _cpu;
					Drawing::Image * _image;

					int ConvertKeyAwToChip8(Keys::Enum key);
					void CheckBackcolor();
					void UpdateImage();

				public:
					Chip8();
					virtual ~Chip8();

					virtual int IsClass(Classes::Enum objectClass) const;
					virtual int GetType() const { return Types::Chip8;}

					void LoadRom(const String file);

					virtual void OnTick();
					virtual void OnPaint(OpenGL::GL* gl);
					int GetChip8Mode();
					void SetInvertedColors(bool mode);

					virtual bool OnKeyPress(Keys::Enum key);
					virtual bool OnKeyUp(Keys::Enum key);
			};
		}
	}
}
