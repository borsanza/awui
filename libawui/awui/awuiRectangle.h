// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUIRECTANGLE_H__
#define __AWUIRECTANGLE_H__

#include "awuiPoint.h"
#include "awuiSize.h"

class awuiRectangle {
private:
	awuiPoint location;
	awuiSize size;

public:
	awuiRectangle();
	awuiRectangle(const awuiPoint location, const awuiSize size);
	awuiRectangle(int x, int y, int width, int height);
	
	int GetWidth() const;
	void SetWidth(int width);

	int GetHeight() const;
	void SetHeight(int height);

	int GetX() const;
	void SetX(int x);

	int GetY() const;
	void SetY(int y);

	int GetBottom() const;
	int GetLeft() const;
	int GetRight() const;
	int GetTop() const;
	awuiPoint GetLocation() const;
	awuiSize GetSize() const;
	void SetLocation(const awuiPoint location);
	void SetSize(const awuiSize size);

	void Inflate(const awuiSize size);
	void Inflate(int width, int height);

	void Offset(const awuiPoint pos);
	void Offset(int x, int y);

	static awuiRectangle FromLTRB(int left, int top, int right, int bottom);
	static awuiRectangle Intersect(const awuiRectangle rectangle1, const awuiRectangle rectangle2);
	void Intersect(const awuiRectangle rectangle);

	awuiRectangle & operator= (const awuiRectangle & other);
};

#endif
