#ifndef _AWUI_EMULATION_MASTERSYSTEM_REGISTERS_H
#define _AWUI_EMULATION_MASTERSYSTEM_REGISTERS_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
/*
 * Flags of F
 * 7  6  5  4  3  2   1  0
 * S  Z     H     PV  N  C
 *
 * S: Sign flag. Set if the 2-complement value is negative (copy of MSB)
 * Z: Zero flag. Set if the value is zero
 * H: Half Carry. Carry from bit 3 to bit 4
 * PV: Parity or Overflow. Parity set if even number of bits set. Overflow set if the 2-complement result does not fit in the register
 * N: Subtract. Set if the last operation was a subtraction
 * C: Carry. Set if the result did not fit in the register
 */
			enum {
				Flag_C  = 0x01,
				Flag_N  = 0x02,
				Flag_P  = 0x04,
				Flag_V  = 0x04,
				Flag_F3 = 0x08, // <- Undocumented
				Flag_H  = 0x10,
				Flag_F5 = 0x20, // <- Undocumented
				Flag_Z  = 0x40,
				Flag_S  = 0x80,

				Flag_i_C = 0xFE,
				Flag_i_N = 0xFD,
				Flag_i_P = 0xFB,
				Flag_i_V = 0xFB,
				Flag_i_H = 0xEF,
				Flag_i_Z = 0xBF,
				Flag_i_S = 0x7F,
			};

			enum {
				Reg_B = 10,
				Reg_C = 11,
				Reg_D = 12,
				Reg_E = 13,
				Reg_H = 14,
				Reg_L = 15,
				Reg_A = 17,
				Reg_F = 18,
				Reg_R = 19,
				Reg_IXH = 20,
				Reg_IXL = 21,
				Reg_IYH = 22,
				Reg_IYL = 23,
			};

			enum {
				Reg_BC = 30,
				Reg_DE = 31,
				Reg_HL = 32,
				Reg_SP = 33,
				Reg_IX = 34,
				Reg_IY = 35,
				Reg_AF = 36,
			};

			static uint8_t ZS_Flags[256] = {
				Flag_Z, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S,
				Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S,
				Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S,
				Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S,
				Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S,
				Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S,
				Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S,
				Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S, Flag_S
			};

			class Registers {
				private:
					uint8_t _a;
					uint8_t _f;
					uint16_t _bc;
					uint16_t _de;
					uint16_t _hl;
					uint8_t _a_;
					uint8_t _f_;
					uint16_t _bc_;
					uint16_t _de_;
					uint16_t _hl_;
					uint16_t _ix;
					uint16_t _iy;
					uint16_t _pc;
					uint16_t _sp;
					uint8_t _im;
					uint8_t _r;
					bool _iff1;
					bool _iff2;
					bool _interruptsEnabled;

				public:
					Registers();
					~Registers();

					void Clear();
					void Alternate();
					void AlternateAF();
					void AlternateBC();
					void AlternateDE();
					void AlternateHL();

					void SetInterruptsEnabled(bool mode);
					bool GetInterruptsEnabled() const;

					void SetSP(uint16_t value);
					uint16_t GetSP() const;

					inline uint16_t GetPC() const     { return this->_pc; }
					inline void IncPC()               { this->_pc++; }
					inline void IncPC(int16_t value)  { this->_pc += value; }
					inline void SetPC(uint16_t value) { this->_pc = value; }

					void SetIFF1(bool mode);
					void SetIFF2(bool mode);
					bool GetIFF1() const;
					bool GetIFF2() const;

					uint16_t GetAF() const;
					void SetA(uint8_t value);
					void SetF(uint8_t value);
					uint8_t GetA() const;
					uint8_t GetF() const;

					void SetR(uint8_t value);
					uint8_t GetR() const;

					void SetBC(uint16_t value);
					void SetB(uint8_t value);
					void SetC(uint8_t value);
					uint16_t GetBC() const;
					uint8_t GetB() const;
					uint8_t GetC() const;

					void SetDE(uint16_t value);
					void SetD(uint8_t value);
					void SetE(uint8_t value);
					uint16_t GetDE() const;
					uint8_t GetD() const;
					uint8_t GetE() const;

					void SetHL(uint16_t value);
					void SetH(uint8_t value);
					void SetL(uint8_t value);
					uint16_t GetHL() const;
					uint8_t GetH() const;
					uint8_t GetL() const;

					void SetIX(uint16_t value);
					uint16_t GetIX() const;

					void SetIY(uint16_t value);
					uint16_t GetIY() const;

					void SetIM(uint8_t mode);
					uint8_t GetIM();

					void SetFFlag(uint8_t flag, bool value);

					uint8_t GetRegm(uint8_t reg);
					uint16_t GetRegss(uint8_t reg);
					void SetRegm(uint8_t reg, uint8_t value);
					void SetRegss(uint8_t reg, uint16_t value);
			};
		}
	}
}

#endif
