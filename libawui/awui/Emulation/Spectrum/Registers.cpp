/*
 * awui/Emulation/Spectrum/Registers.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Registers.h"

#include <assert.h>

using namespace awui::Emulation::Spectrum;

Registers::Registers() {
	this->Clear();
}

Registers::~Registers() {
}

void Registers::Clear() {
	this->_af.W = 0;
	this->_bc.W = 0;
	this->_de.W = 0;
	this->_hl.W = 0;
	this->_af_.W = 0;
	this->_bc_.W = 0;
	this->_de_.W = 0;
	this->_hl_.W = 0;
	this->_ix.W = 0;
	this->_iy.W = 0;
	this->_pc.W = 0;
	this->_sp.W = 0xDFF0;
	this->_im = 0;
	this->_iff1 = false;
	this->_iff2 = false;
}

void Registers::Alternate() {
	this->AlternateAF();
	this->AlternateBC();
	this->AlternateDE();
	this->AlternateHL();
}

void Registers::AlternateAF() {
	uint16_t aux = this->_af.W;
	this->_af.W = this->_af_.W;
	this->_af_.W = aux;
}

void Registers::AlternateBC() {
	uint16_t aux = this->_bc.W;
	this->_bc.W = this->_bc_.W;
	this->_bc_.W = aux;
}

void Registers::AlternateDE() {
	uint16_t aux = this->_de.W;
	this->_de.W = this->_de_.W;
	this->_de_.W = aux;
}

void Registers::AlternateHL() {
	uint16_t aux = this->_hl.W;
	this->_hl.W = this->_hl_.W;
	this->_hl_.W = aux;
}

void Registers::SetFFlag(uint8_t flag, bool value) {
	if (value)
		this->_af.L |= flag;
	else
		this->_af.L &= ~flag;
}

uint8_t Registers::GetRegm(uint8_t reg) const {
	switch (reg) {
		case Reg_B: return this->_bc.H; break;
		case Reg_C: return this->_bc.L; break;
		case Reg_D: return this->_de.H; break;
		case Reg_E: return this->_de.L; break;
		case Reg_H: return this->_hl.H; break;
		case Reg_L: return this->_hl.L; break;
		case Reg_A: return this->_af.H; break;
		case Reg_F: return this->_af.L; break;
		case Reg_I: return this->_ir.H; break;
		case Reg_R: return this->_ir.L; break;
		case Reg_IXH: return this->_ix.H; break;
		case Reg_IXL: return this->_ix.L; break;
		case Reg_IYH: return this->_iy.H; break;
		case Reg_IYL: return this->_iy.L; break;
		default: assert(0); break;
	}

	return 0;
}

void Registers::SetRegm(uint8_t reg, uint8_t value) {
	switch (reg) {
		case Reg_B: this->_bc.H = value; break;
		case Reg_C: this->_bc.L = value; break;
		case Reg_D: this->_de.H = value; break;
		case Reg_E: this->_de.L = value; break;
		case Reg_H: this->_hl.H = value; break;
		case Reg_L: this->_hl.L = value; break;
		case Reg_A: this->_af.H = value; break;
		case Reg_F: this->_af.L = value; break;
		case Reg_I: this->_ir.H = value; break;
		case Reg_R: this->_ir.L = value; break;
		case Reg_IXH: this->_ix.H = value; break;
		case Reg_IXL: this->_ix.L = value; break;
		case Reg_IYH: this->_iy.H = value; break;
		case Reg_IYL: this->_iy.L = value; break;
		default: assert(0); break;
	}
}

uint16_t Registers::GetRegss(uint8_t reg) const {
	switch (reg) {
		case Reg_BC: return this->_bc.W;
		case Reg_DE: return this->_de.W;
		case Reg_HL: return this->_hl.W;
		case Reg_SP: return this->_sp.W;
		case Reg_IX: return this->_ix.W;
		case Reg_IY: return this->_iy.W;
		case Reg_AF: return this->_af.W;
		case Reg_PC: return this->_pc.W;
		default: assert(0);
	}

	return 0;
}

void Registers::SetRegss(uint8_t reg, uint16_t value) {
	switch (reg) {
		case Reg_BC: this->_bc.W = value; break;
		case Reg_DE: this->_de.W = value; break;
		case Reg_HL: this->_hl.W = value; break;
		case Reg_SP: this->_sp.W = value; break;
		case Reg_IX: this->_ix.W = value; break;
		case Reg_IY: this->_iy.W = value; break;
		case Reg_AF: this->_af.W = value; break;
		case Reg_PC: this->_pc.W = value; break;
		default: assert(0); break;
	}
}
