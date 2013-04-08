// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Math.h>
#include <cmath>
#include <stdlib.h>

double awui::Math::Abs(double value) {
	return fabs(value);
}

float awui::Math::Abs(float value) {
	return fabsf(value);
}

int awui::Math::Abs(int value) {
	return abs(value);
}

double awui::Math::Acos(double d) {
	return acos(d);
}

float awui::Math::Acos(float d) {
	return acosf(d);
}

double awui::Math::Asin(double d) {
	return asin(d);
}

float awui::Math::Asin(float d) {
	return asinf(d);
}

double awui::Math::Atan(double d) {
	return atan(d);
}

float awui::Math::Atan(float d) {
	return atanf(d);
}

double awui::Math::Atan2(double y, double x) {
	return atan2(y, x);
}

float awui::Math::Atan2(float y, float x) {
	return atan2f(y, x);
}

double awui::Math::Ceiling(double a) {
	return ceil(a);
}

float awui::Math::Ceiling(float a) {
	return ceilf(a);
}

double awui::Math::Cos(double d) {
	return cos(d);
}

float awui::Math::Cos(float d) {
	return cosf(d);
}

double awui::Math::Cosh(double value) {
	return cosh(value);
}

float awui::Math::Cosh(float value) {
	return coshf(value);
}

double awui::Math::Exp(double d) {
	return exp(d);
}

float awui::Math::Exp(float d) {
	return expf(d);
}

double awui::Math::Floor(double d) {
	return floor(d);
}

float awui::Math::Floor(float d) {
	return floorf(d);
}

double awui::Math::FMod(double x, double y) {
	return fmod(x, y);
}

float awui::Math::FMod(float x, float y) {
	return fmodf(x, y);
}

double awui::Math::Log(double d) {
	return log(d);
}

float awui::Math::Log(float d) {
	return logf(d);
}

double awui::Math::Log10(double d) {
	return log10(d);
}

float awui::Math::Log10(float d) {
	return log10f(d);
}

double awui::Math::Max(double val1, double val2) {
	return fmax(val1, val2);
}

float awui::Math::Max(float val1, float val2) {
	return fmaxf(val1, val2);
}

int awui::Math::Max(int val1, int val2) {
	return val1 > val2 ? val1 : val2;
}

double awui::Math::Min(double val1, double val2) {
	return fmin(val1, val2);
}

float awui::Math::Min(float val1, float val2) {
	return fminf(val1, val2);
}

int awui::Math::Min(int val1, int val2) {
	return val1 < val2 ? val1 : val2;
}

double awui::Math::Pow(double x, float y) {
	return pow(x, y);
}

float awui::Math::Pow(float x, float y) {
	return powf(x, y);
}

double awui::Math::Round(double x) {
	return round(x);
}

float awui::Math::Round(float x) {
	return roundf(x);
}

int awui::Math::Sign(double value) {
	return (value == 0)? 0 :((value < 0)?-1 : 1);
}

int awui::Math::Sign(float value){
	return (value == 0)? 0 :((value < 0)?-1 : 1);
}

int awui::Math::Sign(short value) {
	return (value == 0)? 0 :((value < 0)?-1 : 1);
}

int awui::Math::Sign(long value) {
	return (value == 0)? 0 :((value < 0)?-1 : 1);
}

int awui::Math::Sign(int value) {
	return (value == 0)? 0 :((value < 0)?-1 : 1);
}

double awui::Math::Sin(double a) {
	return sin(a);
}

float awui::Math::Sin(float a) {
	return sinf(a);
}

double awui::Math::Sinh(double value) {
	return sinh(value);
}

float awui::Math::Sinh(float value) {
	return sinhf(value);
}

double awui::Math::Sqrt(double d) {
	return sqrt(d);
}

float awui::Math::Sqrt(float d) {
	return sqrtf(d);
}

double awui::Math::Tan(double a) {
	return tan(a);
}

float awui::Math::Tan(float a) {
	return tanf(a);
}

double awui::Math::Tanh(double value) {
	return tanh(value);
}

float awui::Math::Tanh(float value) {
	return tanhf(value);
}
