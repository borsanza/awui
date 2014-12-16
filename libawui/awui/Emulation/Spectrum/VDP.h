#ifndef _AWUI_EMULATION_SPECTRUM_VDP_H
#define _AWUI_EMULATION_SPECTRUM_VDP_H

#include <stdint.h>

// Segun documentacion
#define LEFTBORDER 13
#define RIGHTBORDER 15

// Comparado con la Master System
//#define LEFTBORDER 1
//#define RIGHTBORDER 12

namespace awui {
	namespace Emulation {
		namespace Common {
			class Ram;
		};

		namespace Spectrum {
			class CPU;
			enum {
				SPRITE_16x16 = 1,
				SPRITE_8x16 = 2,
				SPRITE_8x8 = 3,
			};

			class VDP {
				private:
					struct saveData {
						uint16_t _width;
						uint16_t _height;
						uint16_t _line;
						uint16_t _col;
						uint16_t _baseAddress;
						uint16_t _dataByte;
						uint16_t _address;
						uint8_t _portState;
						uint8_t _readbuffer;
						uint8_t _verticalScroll;
						uint8_t _status;
						uint8_t _spriteSize;
						bool _controlMode:1;
						bool _interrupt:1;
						bool _ntsc:1;
						bool _showBorder:1;
						bool _visible:1;
						uint8_t _cram[32];
						uint8_t _registers[11];
						uint8_t _vram[16384];
						uint16_t _sizeData;
						uint8_t _data[(256 + LEFTBORDER + RIGHTBORDER) * 294]; // 294 parece que es el maximo en alto
					} d;

					// No salvable
					CPU * _cpu;

					uint8_t NTSCx192[262];
					uint8_t NTSCx224[262];
					uint8_t NTSCx240[262];
					uint8_t PALx192[313];
					uint8_t PALx224[313];
					uint8_t PALx240[313];
					uint8_t HORSYNC[342];

					uint8_t GetStatus(bool resetStatus = true);
					void UpdateAllRegisters();

					void ResetVideo();
					void SetHeight(uint16_t height);
					void CalcNextPixel(uint16_t * col, uint16_t * line, bool * hsync, bool * vsync);
					bool IsVSYNC(uint16_t line) const;

					uint8_t GetSpriteColor(uint16_t sprite, int x, int y, bool flipx, bool flipy, bool otherPalete, bool doble) const;
					bool GetSpritePixel(uint8_t * color) const;
					uint8_t GetBackgroundPixel(uint16_t sprite, int16_t x, int16_t y, bool flipx, bool flipy, bool otherPalete, bool bgPriority) const;

				public:
					VDP(CPU * cpu);
					~VDP();

					void OnTickBorder();
					bool OnTick(uint32_t counter);

					void WriteControlByte(uint8_t value);
					void WriteDataByte(uint8_t value);

					void WriteByte(uint8_t port, uint8_t value);
					uint8_t ReadByte(uint8_t port);

					uint16_t GetWidth() const;
					uint16_t GetHeight() const;

					void Reset();

					const uint8_t * GetColors() const;
					uint8_t * GetVram();

					void SetNTSC();
					void SetPAL();
					bool GetNTSC() const;
					bool GetPAL() const;
					uint16_t GetTotalWidth() const;
					uint16_t GetTotalHeight() const;
					uint16_t GetVisualWidth() const;
					uint16_t GetVisualHeight() const;
					uint16_t GetActiveTop() const;
					uint16_t GetActiveLeft() const;
					uint16_t GetBorderBottom() const;

					uint8_t GetPixel(uint16_t x, uint16_t y) const;

					bool GetShowBorder() const;
					void SetShowBorder(bool mode);

					void Clear();

					bool GetInterrupt();

					uint16_t GetLine() const;
					uint16_t GetColumn() const;

					uint8_t GetBackColor() const;

					static int GetSaveSize();

					void LoadState(uint8_t * data);
					void SaveState(uint8_t * data);
			};
		}
	}
}

#endif
