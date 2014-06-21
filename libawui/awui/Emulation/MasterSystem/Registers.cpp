/*
 * awui/Emulation/MasterSystem/Registers.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Registers.h"

using namespace awui::Emulation::MasterSystem;

/*
 * Flags of F
 * 7  6  5  4  3  2   1  0
 * S  Z     H     PV  N  C
 */

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
	this->_iff1 = false;
	this->_iff2 = false;
}

void Registers::Alternate() {
	uint8_t aux1;
	aux1 = this->_a;
	this->_a = this->_a_;
	this->_a_ = aux1;

	aux1 = this->_f;
	this->_f = this->_f_;
	this->_f_ = aux1;

	uint8_t aux2;
	aux2 = this->_bc;
	this->_bc = this->_bc_;
	this->_bc_ = aux2;

	aux2 = this->_de;
	this->_de = this->_de_;
	this->_de_ = aux2;

	aux2 = this->_hl;
	this->_hl = this->_hl_;
	this->_hl_ = aux2;
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

void Registers::IncPC(uint8_t value) {
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