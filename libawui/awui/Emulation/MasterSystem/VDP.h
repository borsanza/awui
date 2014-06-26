#ifndef _AWUI_EMULATION_MASTERSYSTEM_VDP_H
#define _AWUI_EMULATION_MASTERSYSTEM_VDP_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class CPU;
			class Ram;

			class VDP {
				private:
					CPU * _cpu;
					uint16_t _width;
					uint16_t _height;

					uint16_t _line;
					uint16_t _col;
					uint8_t _status;

					// 32 Color Ram
					uint8_t _cram[32];

					// 11 Registers
					uint8_t _registers[11];

					int16_t _controlByte;
					uint16_t _dataByte;
					uint16_t _address;
					bool _goVram;

					Ram * _vram;

					uint8_t GetStatus();

				public:
					VDP(CPU * cpu);
					~VDP();

					bool OnTick();

					void WriteControlByte(uint8_t value);
					void WriteDataByte(uint8_t value);

					void WriteByte(uint8_t port, uint8_t value);
					uint8_t ReadByte(uint8_t port);
			};
		}
	}
}

#endif