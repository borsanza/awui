/*
 * awui/Random.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Random.h"

#include <awui/Math.h>
#include <stdlib.h>

using namespace awui;

Random::Random() {
	this->seed = 0;
}

Random::Random(unsigned int seed) {
	this->seed = seed;
}

Random::~Random() {
}

int Random::Next() {
	if (this->seed)
		return rand_r(&this->seed);

	return rand();
}

/**
[0, max[
max es exclusivo
*/
int Random::Next(int exclusive_max) {
	return this->Next(0, exclusive_max);
}

/**
[min, max[
max es exclusivo
*/
int Random::Next(int min, int exclusive_max) {
	double value = (double) this->Next() / RAND_MAX;
	return min + Math::Round(value * ((exclusive_max - 1) - min));
}

int Random::NextByte() {
	return this->Next(0, 256);
}