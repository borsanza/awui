#ifndef _AWUI_EMULATION_SPECTRUM_ULA_H
#define _AWUI_EMULATION_SPECTRUM_ULA_H

#include <stdint.h>

// Me he olvidado de los 8 de vsync para que no se vea negro en la imagen,
// eso hay que descontarlo en los retrazos

#define SPECTRUM_BORDER_WIDTH 48
#define SPECTRUM_BORDER_HEIGHT_TOP 56
#define SPECTRUM_BORDER_HEIGHT_BOTTOM 56
#define SPECTRUM_WIDTH 352  // 352 = 48 + 256 + 48
#define SPECTRUM_HEIGHT 304 // 312 =  8 + 56 + 192 + 56

namespace awui {
	namespace Emulation {
		namespace Spectrum {
			class Motherboard;

			class ULA {
				private:
					struct saveData {
						uint16_t _width;
						uint16_t _height;
						uint16_t _line;
						uint16_t _col;
						bool _interrupt:1;
						uint8_t _vram[16384];
						uint8_t _data[SPECTRUM_WIDTH * SPECTRUM_HEIGHT];
						uint8_t _backcolor;
						uint8_t _lastbackcolor;
						int _blinkCount;
						bool _blink;
					} d;

					void SetHeight(uint16_t height);
					void CalcNextPixel(uint16_t * col, uint16_t * line, bool * vsync);
					bool IsVSYNC(uint16_t line) const;

					void OnTickBorder();
					uint16_t GetWidth() const;
					uint16_t GetHeight() const;

				public:
					ULA();
					~ULA();

					bool OnTick(uint32_t counter);

					void Reset();

					uint8_t * GetVram();

					uint16_t GetTotalWidth() const;
					uint16_t GetTotalHeight() const;

					uint8_t GetPixel(uint16_t x, uint16_t y) const;

					void Clear();

					bool GetInterrupt();

					uint16_t GetLine() const;
					uint16_t GetColumn() const;

					static int GetSaveSize();

					void LoadState(uint8_t * data);
					void SaveState(uint8_t * data);

					inline uint8_t GetBackColor() const { return this->d._backcolor; }
					inline void SetBackColor(uint8_t color) { this->d._backcolor = color; }
					inline uint8_t ReadByte(uint32_t pos) const { return this->d._vram[pos]; }
					inline void WriteByte(uint32_t pos, uint8_t value) { this->d._vram[pos] = value; }
			};
		}
	}
}

#endif
