/*
 * awui/Effects/Effect.cpp
 *
 * Copyright (C) 2011 Borja Sánchez Zamorano
 */

#include "Effect.h"

#include <awui/Math.h>

using namespace awui::Effects;

Effect::Effect(String name1) {
	this->name = name1;
}

awui::String Effect::GetName() {
	return this->name;
}

float EffectLinear::Calculate(float p) const {
	return p;
}

float EffectSwing::Calculate(float p) const {
	return 0.5f - Math::Cos(p * Math::PI) / 2.0f;
}

float EffectQuad::Calculate(float p) const {
	return Math::Pow(p, 2.0f);
}

float EffectCubic::Calculate(float p) const {
	return Math::Pow(p, 3.0f);
}

float EffectQuart::Calculate(float p) const {
	return Math::Pow(p, 4.0f);
}

float EffectQuint::Calculate(float p) const {
	return Math::Pow(p, 5.0f);
}

float EffectExpo::Calculate(float p) const {
	return Math::Pow(p, 6.0f);
}

float EffectSine::Calculate(float p) const {
	return 1.0f - Math::Cos(p * Math::PI / 2.0f);
}

float EffectCirc::Calculate(float p) const {
	return 1.0f - Math::Sqrt(1.0f - p * p);
}

float EffectElastic::Calculate(float p) const {
	return ((p == 0.0f) || (p == 1.0f)) ? p : -Math::Pow(2.0f, 8.0f * (p - 1.0f)) * Math::Sin(((p - 1.0f) * 80.0f - 7.5f) * Math::PI / 15.0f);
}

float EffectBack::Calculate(float p) const {
	return p * p * (3.0f * p - 2.0f);
}

float EffectBounce::Calculate(float p) const {
	float pow2, bounce = 4.0f;

	while (p < ((pow2 = Math::Pow(2.0f, --bounce)) - 1.0f) / 11.0f) {
	}
	return 1.0f / Math::Pow(4.0f, 3.0f - bounce) - 7.5625f * Math::Pow((pow2 * 3.0f - 2.0f) / 22.0f - p, 2.0f);
}

float EffectIn::Calculate(float p, Effect *effect) const {
	return effect->Calculate(p);
}

float EffectOut::Calculate(float p, Effect *effect) const {
	return 1.0f - effect->Calculate(1.0f - p);
}

float EffectInOut::Calculate(float p, Effect *effect) const {
	if (p < 0.5f)
		return effect->Calculate(p * 2.0f) / 2.0f;

	return 1.0f - effect->Calculate((p * -2.0f) + 2.0f) / 2.0f;
}