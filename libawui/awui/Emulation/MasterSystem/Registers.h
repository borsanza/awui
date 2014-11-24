#ifndef _AWUI_EMULATION_MASTERSYSTEM_REGISTERS_H
#define _AWUI_EMULATION_MASTERSYSTEM_REGISTERS_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		union Word {
			uint16_t W;
			struct {
				uint8_t L;
				uint8_t H;
			};
		};

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
				Reg_PC = 37,
			};

			constexpr static uint8_t ZS_Flags[256] = {
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
					Word _af;
					Word _bc;
					Word _de;
					Word _hl;
					Word _af_;
					Word _bc_;
					Word _de_;
					Word _hl_;
					Word _ix;
					Word _iy;
					Word _pc;
					Word _sp;
					uint8_t _im;
					uint8_t _r;
					bool _iff1;
					bool _iff2;

				public:
					Registers();
					~Registers();

					void Clear();
					void Alternate();
					void AlternateAF();
					void AlternateBC();
					void AlternateDE();
					void AlternateHL();

					inline uint8_t GetR() const       { return this->_r; }
					inline void SetR(uint8_t value)   { this->_r = value; }

					inline uint8_t GetIM() const      { return this->_im; }
					inline void SetIM(uint8_t value)  { this->_im = value; }

					inline uint16_t GetSP() const     { return this->_sp.W; }
					inline void SetSP(uint16_t value) { this->_sp.W = value; }

					inline uint16_t GetPC() const     { return this->_pc.W; }
					inline void SetPC(uint16_t value) { this->_pc.W = value; }
					inline void IncPC()               { this->_pc.W++; }
					inline void IncPC(int16_t value)  { this->_pc.W += value; }

					inline bool GetIFF1() const       { return this->_iff1; }
					inline bool GetIFF2() const       { return this->_iff2; }
					inline void SetIFF1(bool mode)    { this->_iff1 = mode; }
					inline void SetIFF2(bool mode)    { this->_iff2 = mode; }

					inline uint16_t GetAF() const     { return this->_af.W; }
					inline uint8_t GetA() const       { return this->_af.H; }
					inline uint8_t GetF() const       { return this->_af.L; }
					inline void SetAF(uint16_t value) { this->_af.W = value; }
					inline void SetA(uint8_t value)   { this->_af.H = value; }
					inline void SetF(uint8_t value)   { this->_af.L = value; }

					inline uint16_t GetBC() const     { return this->_bc.W; }
					inline uint8_t GetB() const       { return this->_bc.H; }
					inline uint8_t GetC() const       { return this->_bc.L; }
					inline void SetBC(uint16_t value) { this->_bc.W = value; }
					inline void SetB(uint8_t value)   { this->_bc.H = value; }
					inline void SetC(uint8_t value)   { this->_bc.L = value; }

					inline uint16_t GetDE() const     { return this->_de.W; }
					inline uint8_t GetD() const       { return this->_de.H; }
					inline uint8_t GetE() const       { return this->_de.L; }
					inline void SetDE(uint16_t value) { this->_de.W = value; }
					inline void SetD(uint8_t value)   { this->_de.H = value; }
					inline void SetE(uint8_t value)   { this->_de.L = value; }

					inline uint16_t GetHL() const     { return this->_hl.W; }
					inline uint8_t GetH() const       { return this->_hl.H; }
					inline uint8_t GetL() const       { return this->_hl.L; }
					inline void SetHL(uint16_t value) { this->_hl.W = value; }
					inline void SetH(uint8_t value)   { this->_hl.H = value; }
					inline void SetL(uint8_t value)   { this->_hl.L = value; }

					inline uint16_t GetIX() const     { return this->_ix.W; }
					inline uint8_t GetIXH() const     { return this->_ix.H; }
					inline uint8_t GetIXL() const     { return this->_ix.L; }
					inline void SetIX(uint16_t value) { this->_ix.W = value; }
					inline void SetIXH(uint8_t value) { this->_ix.H = value; }
					inline void SetIXL(uint8_t value) { this->_ix.L = value; }

					inline uint16_t GetIY() const     { return this->_iy.W; }
					inline uint8_t GetIYH() const     { return this->_iy.H; }
					inline uint8_t GetIYL() const     { return this->_iy.L; }
					inline void SetIY(uint16_t value) { this->_iy.W = value; }
					inline void SetIYH(uint8_t value) { this->_iy.H = value; }
					inline void SetIYL(uint8_t value) { this->_iy.L = value; }

					void SetFFlag(uint8_t flag, bool value);

					uint8_t GetRegm(uint8_t reg) const;
					void SetRegm(uint8_t reg, uint8_t value);

					uint16_t GetRegss(uint8_t reg) const;
					void SetRegss(uint8_t reg, uint16_t value);
			};
		}
	}
}

#endif
