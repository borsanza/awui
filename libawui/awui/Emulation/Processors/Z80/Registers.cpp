/**
 * awui/Emulation/Processors/Z80/Registers.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Registers.h"

#include <assert.h>

using namespace awui::Emulation::Processors::Z80;

Registers::Registers() {
	m_im = 0;
	m_iff1 = false;
	m_iff2 = false;
	Clear();
}

Registers::~Registers() {
}

void Registers::Clear() {
	m_af.W = 0;
	m_bc.W = 0;
	m_de.W = 0;
	m_hl.W = 0;
	m_afAlt.W = 0;
	m_bcAlt.W = 0;
	m_deAlt.W = 0;
	m_hlAlt.W = 0;
	m_ix.W = 0;
	m_iy.W = 0;
	m_pc.W = 0;
	m_sp.W = 0xDFF0;
	m_im = 0;
	m_iff1 = false;
	m_iff2 = false;
}

void Registers::Alternate() {
	AlternateAF();
	AlternateBC();
	AlternateDE();
	AlternateHL();
}

void Registers::AlternateAF() {
	uint16_t aux = m_af.W;
	m_af.W = m_afAlt.W;
	m_afAlt.W = aux;
}

void Registers::AlternateBC() {
	uint16_t aux = m_bc.W;
	m_bc.W = m_bcAlt.W;
	m_bcAlt.W = aux;
}

void Registers::AlternateDE() {
	uint16_t aux = m_de.W;
	m_de.W = m_deAlt.W;
	m_deAlt.W = aux;
}

void Registers::AlternateHL() {
	uint16_t aux = m_hl.W;
	m_hl.W = m_hlAlt.W;
	m_hlAlt.W = aux;
}

void Registers::SetFFlag(uint8_t flag, bool value) {
	if (value)
		m_af.L |= flag;
	else
		m_af.L &= ~flag;
}

uint8_t Registers::GetRegm(uint8_t reg) const {
	switch (reg) {
		case Reg_B:
			return m_bc.H;
			break;
		case Reg_C:
			return m_bc.L;
			break;
		case Reg_D:
			return m_de.H;
			break;
		case Reg_E:
			return m_de.L;
			break;
		case Reg_H:
			return m_hl.H;
			break;
		case Reg_L:
			return m_hl.L;
			break;
		case Reg_A:
			return m_af.H;
			break;
		case Reg_F:
			return m_af.L;
			break;
		case Reg_I:
			return m_ir.H;
			break;
		case Reg_R:
			return m_ir.L;
			break;
		case Reg_IXH:
			return m_ix.H;
			break;
		case Reg_IXL:
			return m_ix.L;
			break;
		case Reg_IYH:
			return m_iy.H;
			break;
		case Reg_IYL:
			return m_iy.L;
			break;
		default:
			assert(0);
			break;
	}

	return 0;
}

void Registers::SetRegm(uint8_t reg, uint8_t value) {
	switch (reg) {
		case Reg_B:
			m_bc.H = value;
			break;
		case Reg_C:
			m_bc.L = value;
			break;
		case Reg_D:
			m_de.H = value;
			break;
		case Reg_E:
			m_de.L = value;
			break;
		case Reg_H:
			m_hl.H = value;
			break;
		case Reg_L:
			m_hl.L = value;
			break;
		case Reg_A:
			m_af.H = value;
			break;
		case Reg_F:
			m_af.L = value;
			break;
		case Reg_I:
			m_ir.H = value;
			break;
		case Reg_R:
			m_ir.L = value;
			break;
		case Reg_IXH:
			m_ix.H = value;
			break;
		case Reg_IXL:
			m_ix.L = value;
			break;
		case Reg_IYH:
			m_iy.H = value;
			break;
		case Reg_IYL:
			m_iy.L = value;
			break;
		default:
			assert(0);
			break;
	}
}

uint16_t Registers::GetRegss(uint8_t reg) const {
	switch (reg) {
		case Reg_BC:
			return m_bc.W;
		case Reg_DE:
			return m_de.W;
		case Reg_HL:
			return m_hl.W;
		case Reg_SP:
			return m_sp.W;
		case Reg_IX:
			return m_ix.W;
		case Reg_IY:
			return m_iy.W;
		case Reg_AF:
			return m_af.W;
		case Reg_PC:
			return m_pc.W;
		default:
			assert(0);
	}

	return 0;
}

void Registers::SetRegss(uint8_t reg, uint16_t value) {
	switch (reg) {
		case Reg_BC:
			m_bc.W = value;
			break;
		case Reg_DE:
			m_de.W = value;
			break;
		case Reg_HL:
			m_hl.W = value;
			break;
		case Reg_SP:
			m_sp.W = value;
			break;
		case Reg_IX:
			m_ix.W = value;
			break;
		case Reg_IY:
			m_iy.W = value;
			break;
		case Reg_AF:
			m_af.W = value;
			break;
		case Reg_PC:
			m_pc.W = value;
			break;
		default:
			assert(0);
			break;
	}
}
