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
				FFlag_C  = 1,
				FFlag_N  = 2,
				FFlag_PV = 4,
				FFlag_H  = 16,
				FFlag_Z  = 64,
				FFlag_S  = 128,
			};

			enum {
				Reg_B = 0,
				Reg_C = 1,
				Reg_D = 2,
				Reg_E = 3,
				Reg_H = 4,
				Reg_L = 5,
				Reg_A = 7,
			};

			enum {
				Reg_BC = 0,
				Reg_DE = 1,
				Reg_HL = 2,
				Reg_SP = 3,
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
					bool _iff1;
					bool _iff2;

				public:
					Registers();
					~Registers();

					void Clear();
					void Alternate();
					void AlternateBC();
					void AlternateDE();
					void AlternateHL();

					void SetSP(uint16_t value);
					uint16_t GetSP() const;

					uint16_t GetPC() const;
					void IncPC();
					void IncPC(int16_t value);
					void SetPC(uint16_t value);

					void SetIFF1(bool mode);
					void SetIFF2(bool mode);

					void SetA(uint8_t value);
					void SetF(uint8_t value);
					uint8_t GetA() const;
					uint8_t GetF() const;

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