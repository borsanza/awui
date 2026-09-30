#pragma once

#include <awui/Emulation/Common/Word.h>
#include <stdint.h>

namespace awui::Emulation::Processors::Z80 {
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
		// clang-format off
		Flag_C  = 0x01,
		Flag_N  = 0x02,
		Flag_P  = 0x04,
		Flag_V  = 0x04,
		Flag_F3 = 0x08, // <- Undocumented
		Flag_H  = 0x10,
		Flag_F5 = 0x20, // <- Undocumented
		Flag_Z  = 0x40,
		Flag_S  = 0x80,
		// clang-format on

		Flag_F3H = 0x0800, // <- Undocumented
		Flag_F5H = 0x2000, // <- Undocumented
	};

	enum {
		Reg_B = 10,
		Reg_C = 11,
		Reg_D = 12,
		Reg_E = 13,
		Reg_H = 14,
		Reg_L = 15,
		Reg_A = 16,
		Reg_F = 17,
		Reg_I = 18,
		Reg_R = 19,
		Reg_IXH = 30,
		Reg_IXL = 31,
		Reg_IYH = 32,
		Reg_IYL = 33,
	};

	enum {
		Reg_BC = 40,
		Reg_DE = 41,
		Reg_HL = 42,
		Reg_SP = 43,
		Reg_IX = 44,
		Reg_IY = 45,
		Reg_AF = 46,
		Reg_PC = 47,
	};

	class Registers {
	  private:
		Word m_af;
		Word m_bc;
		Word m_de;
		Word m_hl;
		Word m_afAlt;
		Word m_bcAlt;
		Word m_deAlt;
		Word m_hlAlt;
		Word m_ix;
		Word m_iy;
		Word m_pc;
		Word m_sp;
		Word m_ir;
		uint8_t m_im;
		bool m_iff1;
		bool m_iff2;

	  public:
		Registers();
		// Sin métodos virtuales: se guarda byte a byte dentro de los estados, y un puntero a la tabla virtual
		// guardado en un fichero no vale en otra ejecución
		~Registers();

		void Clear();
		void Alternate();
		void AlternateAF();
		void AlternateBC();
		void AlternateDE();
		void AlternateHL();

		inline uint8_t GetI() const { return m_ir.H; }
		inline uint8_t GetR() const { return m_ir.L; }
		inline void SetI(uint8_t value) { m_ir.H = value; }
		inline void SetR(uint8_t value) { m_ir.L = value; }

		inline uint8_t GetIM() const { return m_im; }
		inline void SetIM(uint8_t value) { m_im = value; }

		inline uint16_t GetSP() const { return m_sp.W; }
		inline void SetSP(uint16_t value) { m_sp.W = value; }

		inline uint16_t GetPC() const { return m_pc.W; }
		inline void SetPC(uint16_t value) { m_pc.W = value; }
		inline void IncPC() { m_pc.W++; }
		inline void IncPC(int16_t value) { m_pc.W += value; }

		inline bool GetIFF1() const { return m_iff1; }
		inline bool GetIFF2() const { return m_iff2; }
		inline void SetIFF1(bool mode) { m_iff1 = mode; }
		inline void SetIFF2(bool mode) { m_iff2 = mode; }

		inline uint16_t GetAF() const { return m_af.W; }
		inline uint8_t GetA() const { return m_af.H; }
		inline uint8_t GetF() const { return m_af.L; }
		inline void SetAF(uint16_t value) { m_af.W = value; }
		inline void SetA(uint8_t value) { m_af.H = value; }
		inline void SetF(uint8_t value) { m_af.L = value; }

		inline uint16_t GetBC() const { return m_bc.W; }
		inline uint8_t GetB() const { return m_bc.H; }
		inline uint8_t GetC() const { return m_bc.L; }
		inline void SetBC(uint16_t value) { m_bc.W = value; }
		inline void SetB(uint8_t value) { m_bc.H = value; }
		inline void SetC(uint8_t value) { m_bc.L = value; }

		inline uint16_t GetDE() const { return m_de.W; }
		inline uint8_t GetD() const { return m_de.H; }
		inline uint8_t GetE() const { return m_de.L; }
		inline void SetDE(uint16_t value) { m_de.W = value; }
		inline void SetD(uint8_t value) { m_de.H = value; }
		inline void SetE(uint8_t value) { m_de.L = value; }

		inline uint16_t GetHL() const { return m_hl.W; }
		inline uint8_t GetH() const { return m_hl.H; }
		inline uint8_t GetL() const { return m_hl.L; }
		inline void SetHL(uint16_t value) { m_hl.W = value; }
		inline void SetH(uint8_t value) { m_hl.H = value; }
		inline void SetL(uint8_t value) { m_hl.L = value; }

		inline uint16_t GetIX() const { return m_ix.W; }
		inline uint8_t GetIXH() const { return m_ix.H; }
		inline uint8_t GetIXL() const { return m_ix.L; }
		inline void SetIX(uint16_t value) { m_ix.W = value; }
		inline void SetIXH(uint8_t value) { m_ix.H = value; }
		inline void SetIXL(uint8_t value) { m_ix.L = value; }

		inline uint16_t GetIY() const { return m_iy.W; }
		inline uint8_t GetIYH() const { return m_iy.H; }
		inline uint8_t GetIYL() const { return m_iy.L; }
		inline void SetIY(uint16_t value) { m_iy.W = value; }
		inline void SetIYH(uint8_t value) { m_iy.H = value; }
		inline void SetIYL(uint8_t value) { m_iy.L = value; }

		void SetFFlag(uint8_t flag, bool value);

		uint8_t GetRegm(uint8_t reg) const;
		void SetRegm(uint8_t reg, uint8_t value);

		uint16_t GetRegss(uint8_t reg) const;
		void SetRegss(uint8_t reg, uint16_t value);
	};
} // namespace awui::Emulation::Processors::Z80
