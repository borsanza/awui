// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUIPEN_H__
#define __AWUIPEN_H__

#include "awuiObject.h"

class awuiColor;

class awuiLineCap {
public:
	enum LineCap {
		Butt = 0,
		Square = 1,
		Round = 2,
	};
};

class awuiLineJoin {
public:
	enum LineJoin {
		Miter = 0,
		Bevel = 1,
		Round = 2,
	};
};

class awuiPen : public awuiObject {
private:
	awuiColor * color;
	float width;
	awuiLineCap::LineCap lineCap;
	awuiLineJoin::LineJoin lineJoin;

public:
	awuiPen(awuiColor * color);
	awuiPen(awuiColor * color, float width);
	~awuiPen();
	
	virtual int IsClass(awuiClasses objectClass) const;

	awuiColor * GetColor();
	void SetColor(awuiColor * color);

	float GetWidth();
	void SetWidth(float width);

	void SetLineJoin(awuiLineJoin::LineJoin lineJoin);
	awuiLineJoin::LineJoin GetLineJoin();

	void SetLineCap(awuiLineCap::LineCap lineCap);
	awuiLineCap::LineCap GetLineCap();
};

#endif