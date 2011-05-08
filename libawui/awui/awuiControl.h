// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUICONTROL_H__
#define __AWUICONTROL_H__

#include "awuiComponent.h"

class awuiArrayList;
class awuiBitmap;
class awuiColor;
class awuiGraphics;

class awuiControl :	public awuiComponent {
	friend class awuiForm;

protected:
	int x;
	int y;
	int width;
	int height;
	awuiArrayList * controls;
	awuiBitmap * bitmap;
	awuiColor * backColor;

	void OnResizePre();

public:
	awuiControl();
	~awuiControl();

	void SetLeft(int x);
	void SetTop(int y);
	void SetLocation(int x, int y);

	void SetWidth(int width);
	void SetHeight(int height);
	void SetSize(int width, int height);

	void SetBounds(int x, int y, int width, int height);
	
	int GetTop();
	int GetLeft();
	void GetLocation(int &x, int &y);

	int GetWidth();
	int GetHeight();
	void GetSize(int &width, int &height);

	void GetBounds(int &x, int &y, int &width, int &height);

	awuiArrayList * GetControls();

	void SetBackColor(awuiColor * color);
	awuiColor * GetBackColor();

	virtual void OnPaint(awuiGraphics * g) {}
};

#endif