#ifndef _AWUI_EMULATION_MASTERSYSTEM_MOTHERBOARD_H
#define _AWUI_EMULATION_MASTERSYSTEM_MOTHERBOARD_H

#include <awui/Emulation/Common/Word.h>
#include <awui/Emulation/MasterSystem/Ports.h>
#include <awui/Emulation/Processors/Z80/CPU.h>
#include <awui/String.h>
#include <stdint.h>

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

		namespace MasterSystem {
			class VDP;
			class Sound;

			enum {
				MAPPER_NONE = 1,
				MAPPER_SEGA = 2,
			};

			class Motherboard {
				private:
					struct saveData {
						float _frame;
						float _oldFrame;
						uint8_t _controlbyte;
						uint8_t _frame0;
						uint8_t _frame1;
						uint8_t _frame2;
						uint8_t _mapper;
						uint8_t _pad1;
						uint8_t _pad2;
						bool _wantPause:1;
						Word _addressBus;
						Ports _ports;
						uint8_t _boardram[32768];
						uint8_t _ram[8192];
					} d;

					// No se guarda
					bool _showLog:1;
					bool _showLogInt:1;
					bool _showNotImplemented:1;
					Common::Rom * _rom;

					double _initFrame;
					double _percFrame;

					VDP * _vdp;
					Sound * _sound;
					Processors::Z80::CPU _z80;

					void Print(const char * str, ...);
					void CheckInterrupts();

				public:
					Motherboard();
					~Motherboard();

					void LoadRom(const String file);
					void OnTick();
					bool IsHalted() const;

					uint16_t GetAddressBus() const;
					void SetAddressBus(uint16_t);

					void Reset();

					void CallPaused();

					inline VDP * GetVDP() const { return this->_vdp; }
					inline Sound * GetSound() const { return this->_sound; }
					inline void SetPad1(uint8_t pad1) { this->d._pad1 = pad1; }
					inline void SetPad2(uint8_t pad2) { this->d._pad2 = pad2; }
					inline uint8_t GetPad1() const { return this->d._pad1; }
					inline uint8_t GetPad2() const { return this->d._pad2; }

					uint32_t GetCRC32();
					void SetMapper(uint8_t mapper);
					inline uint16_t GetPC() const { return this->_z80.GetPC();}

					static int GetSaveSize();
					void LoadState(uint8_t * data);
					void SaveState(uint8_t * data);

					double GetVirtualTime();
					void RunOpcode();

					void WriteMemory(uint16_t pos, uint8_t value);
					uint8_t ReadMemory(uint16_t pos) const;
					void WritePort(uint8_t port, uint8_t value);
					uint8_t ReadPort(uint8_t port);
			};
		}
	}
}

#endif
