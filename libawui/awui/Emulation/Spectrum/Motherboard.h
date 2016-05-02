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
			class Sound;
			class ULA;

			class Motherboard {
				private:
					struct saveData {
						uint8_t _boardram[32768];
						uint8_t _ram[8192];
						uint8_t _keys[8];
					} d;

					Processors::Z80::CPU * _z80;
					ULA * _ula;
					Sound * _sound;

					// No se guarda
					Common::Rom * _rom;

					double _initFrame;
					double _percFrame;
					int64_t _cycles;
					int64_t _cyclesULA;
					int64_t _lastCycles;
					int64_t _lastCycleMic;
					int64_t _lastMicState;

					void Print(const char * str, ...);
					void CheckInterrupts();

					void (*_writeCassetteCB) (uint16_t data);
					void * _writeCassetteDataCB;
					uint16_t (*_readCassetteCB) ();
					void * _readCassetteDataCB;

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
					inline Sound * GetSound() const { return this->_sound; }

					void WriteMemory(uint16_t pos, uint8_t value);
					uint8_t ReadMemory(uint16_t pos) const;
					void WritePort(uint8_t port, uint8_t value);
					uint8_t ReadPort(uint8_t port) const;

					void OnKeyPress(uint8_t row, uint8_t key);
					void OnKeyUp(uint8_t row, uint8_t key);

					inline ULA * GetULA() const { return this->_ula; }

					void SetWriteCassetteCB(void (* fun)(uint16_t), void * data);
					void SetReadCassetteCB(uint16_t (* fun)(), void * data);
			};
		}
	}
}

#endif
