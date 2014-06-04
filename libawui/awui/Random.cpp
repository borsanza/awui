// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Random.h"

#include <awui/Math.h>
#include <stdlib.h>
#include <time.h>

using namespace awui;

Random::Random() {
	this->seed = (unsigned)time(NULL);
}

Random::Random(unsigned int seed) {
	this->seed = seed;
}

Random::~Random() {
}

int Random::Next() {
	return rand_r(&this->seed);
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
