/*
 * awui/Random.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Random.h"

using namespace awui;

int Random::Next() {
	std::uniform_int_distribution<int> dist;
	return dist(m_rng);
}

/**
[min, max[
max es exclusivo
*/
int Random::Next(int min, int exclusive_max) {
	std::uniform_int_distribution<int> dist(min, exclusive_max - 1);
	return dist(m_rng);
}

/**
[0, max[
max es exclusivo
*/
int Random::Next(int exclusive_max) {
	return Next(0, exclusive_max);
}

uint8_t Random::NextByte() {
	return Next(0, 256);
}
