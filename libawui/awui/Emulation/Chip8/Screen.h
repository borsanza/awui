#ifndef _AWUI_EMULATION_CHIP8_SCREEN_H
#define _AWUI_EMULATION_CHIP8_SCREEN_H

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Screen {
				private:
					int _width;
					int _height;
					unsigned char * _data;

				public:
					Screen(int width, int height);
					~Screen();

					void Clear();

					bool SetPixelXOR(int x, int y, bool value);
					bool SetPixel(int x, int y, int value);
					int GetPixel(int x, int y);

					int GetWidth();
					int GetHeight();

					void ScrollLeft(int columns);
					void ScrollRight(int columns);
					void ScrollUp(int lines);
					void ScrollDown(int lines);
			};
		}
	}
}

#endif