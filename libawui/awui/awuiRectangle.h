// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUIRECTANGLE_H__
#define __AWUIRECTANGLE_H__

#include "awuiPoint.h";
#include "awuiSize.h";

class awuiRectangle {
private:
	awuiPoint location;
	awuiSize size;

public:
	int GetWidth();
	void SetWidth(int width);

	int GetHeight();
	void SetHeight(int height);

	int GetX();
	void SetX(int x);

	int GetY();
	void SetY(int y);

	int GetBottom();
	int GetLeft();
	int GetRight();
	int GetTop();
	awuiPoint GetLocation();
	awuiSize GetSize();
	void SetLocation(awuiPoint location);
	void SetSize(awuiSize size);

	void Intersect(awuiRectangle rectangle);

	awuiRectangle & operator= (const awuiRectangle & other);
};

#endif