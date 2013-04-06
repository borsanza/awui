// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "effects.h"
#include <math.h>

Effect::Effect(const char * name1) {
    this->name = name1;
}

float EffectLinear::calculate(float p)  const {
    return p;
}

float EffectSwing::calculate(float p)  const {
    return 0.5f - cosf(p*M_PI) / 2.0f;
}

float EffectQuad::calculate(float p)  const {
    return powf(p, 2.0f);
}

float EffectCubic::calculate(float p)  const {
    return powf(p, 3.0f);
}

float EffectQuart::calculate(float p)  const {
    return powf(p, 4.0f);
}

float EffectQuint::calculate(float p)  const {
    return powf(p, 5.0f);
}

float EffectExpo::calculate(float p)  const {
    return powf(p, 6.0f);
}

float EffectSine::calculate(float p)  const {
    return 1.0f - cosf( p * M_PI / 2.0f );
}

float EffectCirc::calculate(float p)  const {
    return 1.0f - sqrtf( 1.0f - p * p );
}

float EffectElastic::calculate(float p)  const {
    return ((p == 0.0f) || (p == 1.0f)) ? p :
      -powf(2.0f, 8.0f * (p - 1.0f) ) * sinf( ( (p - 1.0f) * 80.0f - 7.5f ) * M_PI / 15.0f );
}

float EffectBack::calculate(float p)  const {
    return p * p * ( 3.0f * p - 2.0f );
}

float EffectBounce::calculate(float p)  const {
    float pow2, bounce = 4.0f;

    while ( p < ( ( pow2 = powf( 2.0f, --bounce ) ) - 1.0f ) / 11.0f ) {}
    return 1.0f / powf( 4.0f, 3.0f - bounce ) - 7.5625f * powf( ( pow2 * 3.0f - 2.0f ) / 22.0f - p, 2.0f );
}

float EffectIn::calculate(float p, Effect *effect)  const {
    return effect->calculate(p);
}

float EffectOut::calculate(float p, Effect *effect)  const {
    return 1.0f - effect->calculate(1.0f - p);
}

float EffectInOut::calculate(float p, Effect *effect)  const {
    if (p < 0.5f)
        return effect->calculate(p * 2.0f ) / 2.0f;

    return 1.0f - effect->calculate((p * -2.0f) + 2.0f) / 2.0f;
}
