#ifndef _AWUI_EMULATION_SPECTRUM_MOTHERBOARD_H
#define _AWUI_EMULATION_SPECTRUM_MOTHERBOARD_H

#include <awui/String.h>

namespace awui {
	namespace Emulation {
		namespace Common {
			class Rom;
		}

		namespace Processors {
			namespace Z80 {
				class CPU;
			}
		}

		namespace Spectrum {
			class Rom;
			class CPU;

			class Motherboard {
				private:
					struct saveData {
						float _frame;
						float _oldFrame;
						uint8_t _boardram[32768];
						uint8_t _ram[8192];
						uint8_t _ula[16384];
						uint8_t _bgColor;
						uint8_t _keys[8];

						int _blinkCount;
						bool _blink;
					} d;

					// No se guarda
					Common::Rom * _rom;
					Processors::Z80::CPU * _z80;

					double _initFrame;
					double _percFrame;

					void Print(const char * str, ...);
					void CheckInterrupts();

				public:
					Motherboard();
					~Motherboard();

					void LoadRom(const String file);
					void OnTick();

					uint16_t GetAddressBus() const;
					void SetAddressBus(uint16_t);

					void Reset();

					void PrintLog();

					uint32_t GetCRC32();
					void SetMapper(uint8_t mapper);

					static int GetSaveSize();
					void LoadState(uint8_t * data);
					void SaveState(uint8_t * data);

					double GetVirtualTime();

					void WriteMemory(uint16_t pos, uint8_t value);
					uint8_t ReadMemory(uint16_t pos) const;
					void WritePort(uint8_t port, uint8_t value);
					uint8_t ReadPort(uint8_t port) const;

					uint8_t GetBackgroundColor() const { return this->d._bgColor; }

					void OnKeyPress(uint8_t row, uint8_t key);
					void OnKeyUp(uint8_t row, uint8_t key);

					inline bool GetBlink() const { return this->d._blink; }


			};
		}
	}
}

#endif
