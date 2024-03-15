// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Math.h"

#include <math.h>

using namespace awui;

double Math::Abs(double value) {
	return fabs(value);
}

float Math::Abs(float value) {
	return fabsf(value);
}

int Math::Abs(int value) {
	return abs(value);
}

double Math::Acos(double d) {
	return acos(d);
}

float Math::Acos(float d) {
	return acosf(d);
}

double Math::Asin(double d) {
	return asin(d);
}

float Math::Asin(float d) {
	return asinf(d);
}

double Math::Atan(double d) {
	return atan(d);
}

float Math::Atan(float d) {
	return atanf(d);
}

double Math::Atan2(double y, double x) {
	return atan2(y, x);
}

float Math::Atan2(float y, float x) {
	return atan2f(y, x);
}

double Math::Ceiling(double a) {
	return ceil(a);
}

float Math::Ceiling(float a) {
	return ceilf(a);
}

double Math::Cos(double d) {
	return cos(d);
}

float Math::Cos(float d) {
	return cosf(d);
}

double Math::Cosh(double value) {
	return cosh(value);
}

float Math::Cosh(float value) {
	return coshf(value);
}

double Math::Exp(double d) {
	return exp(d);
}

float Math::Exp(float d) {
	return expf(d);
}

double Math::Floor(double d) {
	return floor(d);
}

float Math::Floor(float d) {
	return floorf(d);
}

double Math::FMod(double x, double y) {
	return fmod(x, y);
}

float Math::FMod(float x, float y) {
	return fmodf(x, y);
}

double Math::Log(double d) {
	return log(d);
}

float Math::Log(float d) {
	return logf(d);
}

double Math::Log10(double d) {
	return log10(d);
}

float Math::Log10(float d) {
	return log10f(d);
}

double Math::Max(double val1, double val2) {
	return fmax(val1, val2);
}

float Math::Max(float val1, float val2) {
	return fmaxf(val1, val2);
}

int Math::Max(int val1, int val2) {
	return val1 > val2 ? val1 : val2;
}

double Math::Min(double val1, double val2) {
	return fmin(val1, val2);
}

float Math::Min(float val1, float val2) {
	return fminf(val1, val2);
}

int Math::Min(int val1, int val2) {
	return val1 < val2 ? val1 : val2;
}

double Math::Clamp(double value, double min, double max) {
	return Max(Min(value, max), min);
}

float Math::Clamp(float value, float min, float max) {
	return Max(Min(value, max), min);
}

int Math::Clamp(int value, int min, int max) {
	return Max(Min(value, max), min);
}

double Math::Pow(double x, float y) {
	return pow(x, y);
}

float Math::Pow(float x, float y) {
	return powf(x, y);
}

double Math::Round(double x) {
	return round(x);
}

float Math::Round(float x) {
	return roundf(x);
}

int Math::Sign(double value) {
	return (value == 0) ? 0 : ((value < 0) ? -1 : 1);
}

int Math::Sign(float value) {
	return (value == 0) ? 0 : ((value < 0) ? -1 : 1);
}

int Math::Sign(short value) {
	return (value == 0) ? 0 : ((value < 0) ? -1 : 1);
}

int Math::Sign(long value) {
	return (value == 0) ? 0 : ((value < 0) ? -1 : 1);
}

int Math::Sign(int value) {
	return (value == 0) ? 0 : ((value < 0) ? -1 : 1);
}

double Math::Sin(double a) {
	return sin(a);
}

float Math::Sin(float a) {
	return sinf(a);
}

double Math::Sinh(double value) {
	return sinh(value);
}

float Math::Sinh(float value) {
	return sinhf(value);
}

double Math::Sqrt(double d) {
	return sqrt(d);
}

float Math::Sqrt(float d) {
	return sqrtf(d);
}

double Math::Tan(double a) {
	return tan(a);
}

float Math::Tan(float a) {
	return tanf(a);
}

double Math::Tanh(double value) {
	return tanh(value);
}

float Math::Tanh(float value) {
	return tanhf(value);
}

float Math::Interpolate(float from, float to, float percent, bool rounding) {
	return (rounding && (Round(from) == Round(to))) ? to : from + ((to - from) * percent);
}