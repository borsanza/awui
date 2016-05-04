#ifndef _AWUI_EMULATION_SPECTRUM_ULA_H
#define _AWUI_EMULATION_SPECTRUM_ULA_H

#include <stdint.h>

#define SPECTRUM_VIDEO_WIDTH 256
#define SPECTRUM_VIDEO_WIDTH_RIGHT 48
#define SPECTRUM_VIDEO_WIDTH_SYNC 96
#define SPECTRUM_VIDEO_WIDTH_LEFT 48
#define SPECTRUM_VIDEO_WIDTH_TOTAL 448
#define SPECTRUM_VIDEO_WIDTH_VISUAL 352 // width + left + right

#define SPECTRUM_VIDEO_HEIGHT 192
#define SPECTRUM_VIDEO_HEIGHT_BOTTOM 56
#define SPECTRUM_VIDEO_HEIGHT_SYNC 16
#define SPECTRUM_VIDEO_HEIGHT_TOP 48
#define SPECTRUM_VIDEO_HEIGHT_TOTAL 312
#define SPECTRUM_VIDEO_HEIGHT_VISUAL 296

namespace awui {
	namespace Drawing {
		class Image;
	}

	namespace Emulation {
		namespace Spectrum {
			class Motherboard;

			class ULA {
				private:
					struct saveData {
						uint16_t _line;
						uint16_t _col;
						uint8_t _data[SPECTRUM_VIDEO_WIDTH_TOTAL * SPECTRUM_VIDEO_HEIGHT_TOTAL];
						uint8_t _backcolor;
						uint8_t _lastbackcolor;
						int _blinkCount;
						bool _blink;
					} d;

					const uint32_t _colors[16] = {
						0x000000,
						0x0000BF,
						0xBF0000,
						0xBF00BF,
						0x00BF00,
						0x00BFBF,
						0xBFBF00,
						0xBFBFBF,
						0x000000,
						0x0000FF,
						0xFF0000,
						0xFF00FF,
						0x00FF00,
						0x00FFFF,
						0xFFFF00,
						0xFFFFFF,
					};

					Drawing::Image * _image;
					Motherboard * _motherboard;

					void CalcNextPixel(uint16_t * col, uint16_t * line, bool * hsync, bool * vsync);
					void DrawImage();

				public:
					ULA(Motherboard * motherboard);
					~ULA();

					bool OnTick(uint32_t counter);

					void Reset();

					static int GetSaveSize();

					void LoadState(uint8_t * data);
					void SaveState(uint8_t * data);

					inline void SetBackColor(uint8_t color) { this->d._backcolor = color; }

					Drawing::Image * GetImage() { return this->_image; }
			};
		}
	}
}

#endif
