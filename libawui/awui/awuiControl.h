// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUICONTROL_H__
#define __AWUICONTROL_H__

#include <awui/Object.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Rectangle.h>
#include <string>

class ArrayList;
class awuiBitmap;
class Graphics;
class awuiControlCollection;
class awuiMouseEventArgs;
class awuiGL;

class MouseButtons {
public:
	enum Buttons {
		None = 0,
		Left = 1,
		Right = 2,
		Middle = 4,
		XButton1 = 8,
		XButton2 = 16,
	};
};

using namespace awui;
using namespace awui::Drawing;

class awuiControl : public Object {
	friend class awuiForm;

public:
	enum DockStyle {
		None,
		Top,
		Bottom,
		Left,
		Right,
		Fill,
	};

protected:
	Rectangle bounds;
	Size minimumSize;

	int needRefresh;
	int refreshed;
	awuiControl::DockStyle dock;
	awuiControlCollection * controls;
	awui::Drawing::Color backColor;
	awuiControl * parent;
	awuiMouseEventArgs * mouseEventArgs;
	awuiControl * mouseControl;
	std::string name;

	void OnTickPre();
	void OnResizePre();
	void OnPaintPre(int x, int y, int width, int height, awuiGL * gl);
	void ChangeControlOnMouseOver(awuiControl * control);

public:
	awuiControl();
	~awuiControl();

	virtual int IsClass(Classes objectClass) const;

	const virtual Size GetMinimumSize() const;
	void SetMinimumSize(Size size);

	awuiControl::DockStyle GetDock() const;
	void SetDock(awuiControl::DockStyle dock);

	const std::string& GetName();
	void SetName(const std::string& str);

	int GetLeft() const;
	void SetLeft(int x);

	int GetTop() const;
	void SetTop(int y);

	const Point GetLocation() const;
	void SetLocation(int x, int y);

	int GetWidth() const;
	void SetWidth(int width);

	int GetHeight() const;
	void SetHeight(int height);

	const Size GetSize() const;
	void SetSize(int width, int height);
	void SetSize(const Size size);

	const Rectangle GetBounds() const;
	void SetBounds(int x, int y, int width, int height);

	int GetRight() const;
	int GetBottom() const;

	awuiControlCollection * GetControls() const;

	awui::Drawing::Color GetBackColor();
	void SetBackColor(const awui::Drawing::Color color);

	void Refresh();

	awuiControl * GetParent() const;
	void SetParent(awuiControl * parent);

	void OnMouseMovePre(int x, int y, int buttons);
	void OnMouseUpPre(MouseButtons::Buttons button, int buttons);
	void OnMouseDownPre(int x, int y, MouseButtons::Buttons button, int buttons);

	virtual void Layout();
	virtual void OnMouseDown(awuiMouseEventArgs* e) {}
	virtual void OnMouseMove(awuiMouseEventArgs* e) {}
	virtual void OnMouseUp(awuiMouseEventArgs* e) {}
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();
	virtual void OnPaint(awuiGL * gl) {}
	virtual void OnResize() {}
	virtual void OnTick() {}
};

#endif
