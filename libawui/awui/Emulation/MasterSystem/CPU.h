#ifndef _AWUI_EMULATION_MASTERSYSTEM_CPU_H
#define _AWUI_EMULATION_MASTERSYSTEM_CPU_H

#include <awui/Emulation/MasterSystem/CPUInst.h>
#include <awui/Emulation/MasterSystem/Opcode.h>
#include <awui/String.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class VDP;

			class CPU : public CPUInst {
				private:
					Opcode _opcode;
					float _frame;
					float _oldFrame;
					bool _showLog;
					bool _showLogInt;
					bool _showNotImplemented;
					bool _enableInterrupts;
					bool _inInterrupt;
					bool _isHalted;
					uint8_t _pad1;
					uint8_t _pad2;
					bool _wantPause;

					void Print(const char * str, ...);
					void CheckInterrupts();

				public:
					CPU();
					~CPU();

					inline VDP * GetVDP() const { return this->_vdp; }
					inline void SetPad1(uint8_t pad1) { this->_pad1 = pad1; }
					inline void SetPad2(uint8_t pad2) { this->_pad2 = pad2; }
					inline uint8_t GetPad1() const { return this->_pad1; }
					inline uint8_t GetPad2() const { return this->_pad2; }

					void LoadRom(const String file);
					void RunOpcode();
					void OnTick();
					bool IsHalted() const;

					uint16_t GetAddressBus() const;
					void SetAddressBus(uint16_t);

					inline Registers * GetRegisters() const { return this->_registers; }

					void Reset();

					void PrintLog();

					void CallPaused();
			};
		}
	}
}

#endif
