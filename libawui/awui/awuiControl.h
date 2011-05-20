// (c) Copyright 2011 Borja S�nchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUICONTROL_H__
#define __AWUICONTROL_H__

#include "awuiComponent.h"
#include <string>

class awuiArrayList;
class awuiBitmap;
class awuiColor;
class awuiGraphics;
class awuiControlCollection;

class awuiControl : public awuiComponent {
	friend class awuiForm;

public:
	enum DockStyle {
		None,
		Top,
		Bottom,
		Left,
		Right,
		Fill
	};

protected:
	int x;
	int y;
	int width;
	int height;
	awuiControl::DockStyle dock;
	awuiControlCollection * controls;
	awuiBitmap * bitmap;
	awuiColor * backColor;
	std::string name;

	void OnResizePre();
	void OnPaintPre(awuiGraphics * g);

public:
	awuiControl();
	~awuiControl();
	
	void SetName(const std::string& str);
	const std::string& GetName();

	void SetLeft(int x);
	void SetTop(int y);
	void SetLocation(int x, int y);

	void SetWidth(int width);
	void SetHeight(int height);
	void SetSize(int width, int height);

	void SetBounds(int x, int y, int width, int height);

	int GetTop();
	int GetLeft();
	int GetRight();
	int GetBottom();	
	void GetLocation(int &x, int &y);

	int GetWidth();
	int GetHeight();
	void GetSize(int &width, int &height);

	void GetBounds(int &x, int &y, int &width, int &height);

	awuiArrayList * GetControls();

	void SetBackColor(awuiColor * color);
	awuiColor * GetBackColor();

	void SetDock(awuiControl::DockStyle dock);
	awuiControl::DockStyle GetDock();
	
	virtual void OnPaint(awuiGraphics * g) {}
	virtual void OnResize() {}
	
	void OnMouseMovePre(int x, int y);

	virtual void Layout();
};

#endif
