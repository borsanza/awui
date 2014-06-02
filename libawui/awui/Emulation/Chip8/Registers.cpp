// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Registers.h"

#include <stdlib.h>

Registers::Registers(unsigned char n) {
	this->_length = n;
	this->_v = (unsigned char *) malloc (sizeof(unsigned char *) * n);
	for (unsigned char i = 0; i < n; i++)
		this->_v[i] = 0;

	this->_i = 0;
}

Registers::~Registers() {
	free(this->_v);
}

void Registers::SetV(unsigned char pos, unsigned char value) {
	this->_v[pos] = value;
}

unsigned char Registers::GetV(unsigned char pos) {
	return this->_v[pos];
}

void Registers::SetI(unsigned int value) {
	this->_i = value;
}

unsigned int Registers::GetI() {
	return this->_i;
}
