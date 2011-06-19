// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUICOLOR_H__
#define __AWUICOLOR_H__

#include "awuiObject.h"

class awuiColor : public awuiObject {
private:
	unsigned char a, r, g, b;

public:
	awuiColor();

	virtual int IsClass(awuiClasses objectClass) const;
	
	unsigned char GetA() const;
	unsigned char GetR() const;
	unsigned char GetG() const;
	unsigned char GetB() const;
	int ToArgb() const;
	float GetBrightness() const;
	float GetHue() const;
	float GetSaturation() const;

	static awuiColor FromArgb(int argb);
	static awuiColor FromArgb(int alpha, awuiColor baseColor);
	static awuiColor FromArgb(int red, int green, int blue);
	static awuiColor FromArgb(int alpha, int red, int green, int blue);
	
	awuiColor & operator= (const awuiColor & other);
};

#endif