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
[min, max[
max es exclusivo
*/
int Random::Next(int min, int max) {
	double value = (double) this->Next() / RAND_MAX;
	return min + Math::Round(value * ((max - 1) - min));
}

/**
[0, max[
max es exclusivo
*/
int Random::Next(int max) {
	return this->Next(0, max);
}
