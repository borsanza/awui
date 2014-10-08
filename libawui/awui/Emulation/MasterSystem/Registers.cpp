/*
 * awui/Emulation/MasterSystem/Registers.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Registers.h"

#include <assert.h>
#include <stdio.h>

using namespace awui::Emulation::MasterSystem;

Registers::Registers() {
	this->Clear();
}

Registers::~Registers() {
}

void Registers::Clear() {
	this->_a = 0;
	this->_f = 0;
	this->_bc = 0;
	this->_de = 0;
	this->_hl = 0;
	this->_a_ = 0;
	this->_f_ = 0;
	this->_bc_ = 0;
	this->_de_ = 0;
	this->_hl_ = 0;
	this->_ix = 0;
	this->_iy = 0;
	this->_pc = 0;
	this->_sp = 0;
	this->_im = 0;
	this->_iff1 = false;
	this->_iff2 = false;
	this->_interruptsEnabled = true;
}

void Registers::Alternate() {
	this->AlternateAF();
	this->AlternateBC();
	this->AlternateDE();
	this->AlternateHL();
}

void Registers::AlternateAF() {
	uint8_t aux1;
	aux1 = this->_a;
	this->_a = this->_a_;
	this->_a_ = aux1;

	aux1 = this->_f;
	this->_f = this->_f_;
	this->_f_ = aux1;
}

void Registers::AlternateBC() {
	uint8_t aux = this->_bc;
	this->_bc = this->_bc_;
	this->_bc_ = aux;
}

void Registers::AlternateDE() {
	uint8_t aux = this->_de;
	this->_de = this->_de_;
	this->_de_ = aux;
}

void Registers::AlternateHL() {
	uint8_t aux = this->_hl;
	this->_hl = this->_hl_;
	this->_hl_ = aux;
}

void Registers::SetSP(uint16_t value) {
	this->_sp = value;
}

uint16_t Registers::GetSP() const {
	return this->_sp;
}

uint16_t Registers::GetPC() const {
	return this->_pc;
}

void Registers::IncPC() {
	this->_pc++;
}

void Registers::IncPC(int16_t value) {
	this->_pc += value;
}

void Registers::SetPC(uint16_t value) {
	this->_pc = value;
}

void Registers::SetIFF1(bool mode) {
	this->_iff1 = mode;
}

void Registers::SetIFF2(bool mode) {
	this->_iff2 = mode;
}

bool Registers::GetIFF1() const {
	return this->_iff1;
}

bool Registers::GetIFF2() const {
	return this->_iff2;
}

void Registers::SetA(uint8_t value) {
	this->_a = value;
}

void Registers::SetF(uint8_t value) {
	this->_f = value;
}

uint8_t Registers::GetA() const {
	return this->_a;
}

uint8_t Registers::GetF() const {
	return this->_f;
}

void Registers::SetBC(uint16_t value) {
	this->_bc = value;
}

void Registers::SetB(uint8_t value) {
	*(((uint8_t *) &this->_bc) + 1) = value;
}

void Registers::SetC(uint8_t value) {
	*((uint8_t *) &this->_bc) = value;
}

uint16_t Registers::GetBC() const {
	return this->_bc;
}

uint8_t Registers::GetB() const {
	return *(((uint8_t *) &this->_bc) + 1);
}

uint8_t Registers::GetC() const {
	return this->_bc;
}

void Registers::SetDE(uint16_t value) {
	this->_de = value;
}

void Registers::SetD(uint8_t value) {
	*(((uint8_t *) &this->_de) + 1) = value;
}

void Registers::SetE(uint8_t value) {
	*((uint8_t *) &this->_de) = value;
}

uint16_t Registers::GetDE() const {
	return this->_de;
}

uint8_t Registers::GetD() const {
	return *(((uint8_t *) &this->_de) + 1);
}

uint8_t Registers::GetE() const {
	return this->_de;
}

void Registers::SetHL(uint16_t value) {
	this->_hl = value;
}

void Registers::SetH(uint8_t value) {
	*(((uint8_t *) &this->_hl) + 1) = value;
}

void Registers::SetL(uint8_t value) {
	*((uint8_t *) &this->_hl) = value;
}

uint16_t Registers::GetHL() const {
	return this->_hl;
}

uint8_t Registers::GetH() const {
	return *(((uint8_t *) &this->_hl) + 1);
}

uint8_t Registers::GetL() const {
	return this->_hl;
}

void Registers::SetIX(uint16_t value) {
	this->_ix = value;
}

uint16_t Registers::GetIX() const {
	return this->_ix;
}

void Registers::SetIY(uint16_t value) {
	this->_iy = value;
}

uint16_t Registers::GetIY() const {
	return this->_iy;
}

void Registers::SetIM(uint8_t mode) {
	this->_im = mode;
}

uint8_t Registers::GetIM() {
	return this->_im;
}

void Registers::SetFFlag(uint8_t flag, bool value) {
	if (value)
		this->_f |= flag;
	else
		this->_f &= (0xFF ^ flag);
}

uint8_t Registers::GetRegm(uint8_t reg) {
	switch (reg) {
		case Reg_B: return *(((uint8_t *) &this->_bc) + 1);
		case Reg_C: return this->_bc;
		case Reg_D: return *(((uint8_t *) &this->_de) + 1);
		case Reg_E: return this->_de;
		case Reg_H: return *(((uint8_t *) &this->_hl) + 1);
		case Reg_L: return this->_hl;
		case Reg_A: return this->_a;
		case Reg_F: return this->_f;
	}

	assert(false);
	return 0;
}

void Registers::SetRegm(uint8_t reg, uint8_t value) {
	switch (reg) {
		case Reg_B: *(((uint8_t *) &this->_bc) + 1) = value; break;
		case Reg_C: *((uint8_t *) &this->_bc) = value; break;
		case Reg_D: *(((uint8_t *) &this->_de) + 1) = value; break;
		case Reg_E: *((uint8_t *) &this->_de) = value; break;
		case Reg_H: *(((uint8_t *) &this->_hl) + 1) = value; break;
		case Reg_L: *((uint8_t *) &this->_hl) = value; break;
		case Reg_A: this->_a = value; break;
		case Reg_F: this->_f = value; break;
		default: assert(0);
	}
}

uint16_t Registers::GetRegss(uint8_t reg) {
	switch (reg) {
		case Reg_BC: return this->_bc;
		case Reg_DE: return this->_de;
		case Reg_HL: return this->_hl;
		case Reg_SP: return this->_sp;
		case Reg_IX: return this->_ix;
		case Reg_IY: return this->_iy;
		case Reg_AF: return ((this->_a << 8) | this->_f);
	}

	assert(false);
	return 0;
}

void Registers::SetRegss(uint8_t reg, uint16_t value) {
	switch (reg) {
		case Reg_BC: this->_bc = value; break;
		case Reg_DE: this->_de = value; break;
		case Reg_HL: this->_hl = value; break;
		case Reg_SP: this->_sp = value; break;
		case Reg_IX: this->_ix = value; break;
		case Reg_IY: this->_iy = value; break;
		case Reg_AF:
			this->_a = value >> 8;
			this->_f = value & 0xFF;
			break;
		default: assert(0);
	}
}

void Registers::SetInterruptsEnabled(bool mode) {
	this->_interruptsEnabled = mode;
}

bool Registers::GetInterruptsEnabled() const {
	return this->_interruptsEnabled;
}