// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControl.h"

#include "awuiBitmap.h"
#include "awuiColor.h"
#include "awuiControlCollection.h"
#include "awuiGraphics.h"
#include "awuiMouseEventArgs.h"
#include "awuiPen.h"
#include "awuiRectangle.h"
#include "awuiGL.h"

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

awuiControl::awuiControl() {
	this->controls = new awuiControlCollection(this);
	this->mouseEventArgs = new awuiMouseEventArgs();
	this->mouseControl = NULL;
	this->mouseControlOver = NULL;
	this->parent = NULL;
	this->needRefresh = 1;
	this->x = 0;
	this->y = 0;
	this->width = 100;
	this->height = 100;
	this->bitmap = NULL;
	this->dock = awuiControl::None;
	this->backColor = awuiColor::FromArgb(226, 226, 226);
	this->OnResizePre();
}

awuiControl::~awuiControl() {
	delete this->mouseEventArgs;

	for (int i = 0; i < this->controls->GetCount(); i++)
		delete ((awuiControl *)this->controls->Get(i));

	this->controls->Clear();
	delete this->controls;

	if (this->bitmap != NULL)
		delete this->bitmap;

	if (this->backColor != NULL)
		delete this->backColor;
}

int awuiControl::IsClass(awuiObject::awuiClasses objectClass) {
	if (objectClass == awuiObject::Control)
		return 1;

	return awuiObject::IsClass(objectClass);
}

void awuiControl::SetTop(int y) {
	this->SetLocation(this->x, y);
}

void awuiControl::SetLeft(int x) {
	this->SetLocation(x, this->y);
}

void awuiControl::SetLocation(int x, int y) {
	this->SetBounds(x, y, this->width, this->height);
}

void awuiControl::SetWidth(int width) {
	this->SetSize(width, this->height);
}

void awuiControl::SetHeight(int height) {
	this->SetSize(this->width, height);
}

void awuiControl::SetSize(int width, int height) {
	this->SetBounds(this->x, this->y, width, height);
}

void awuiControl::SetBounds(int x, int y, int width, int height) {
	if ((this->x == x) && (this->y == y) && (this->width == width) && (this->height == height))
		return;

	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->Refresh();

	this->OnResizePre();
}

int awuiControl::GetTop() {
	return this->y;
}

int awuiControl::GetLeft() {
	return this->x;
}

int awuiControl::GetRight() {
	return this->x + this->width - 1;
}

int awuiControl::GetBottom() {
	return this->y + this->height - 1;
}

void awuiControl::GetLocation(int &x, int &y) {
	x = this->GetLeft();
	y = this->GetTop();
}

int awuiControl::GetWidth() {
	return this->width;
}

int awuiControl::GetHeight() {
	return this->height;
}

void awuiControl::GetSize(int &width, int &height) {
	width = this->GetWidth();
	height = this->GetHeight();
}

void awuiControl::GetBounds(int &x, int &y, int &width, int &height) {
	this->GetLocation(x, y);
	this->GetSize(width, height);
}

awuiArrayList * awuiControl::GetControls() {
	return this->controls;
}

void awuiControl::OnResizePre() {
//	if (this->bitmap != NULL)
//		delete this->bitmap;

//	this->bitmap = new awuiBitmap(this->GetWidth(), this->GetHeight());
	
	this->OnResize();
	this->Layout();
}

void awuiControl::SetBackColor(awuiColor * color) {
	if (this->backColor != NULL)
		delete this->backColor;

	this->backColor = awuiColor::FromArgb(color->ToArgb());
}

awuiColor * awuiControl::GetBackColor() {
	return awuiColor::FromArgb(this->backColor->ToArgb());
}

void awuiControl::SetDock(awuiControl::DockStyle dock) {
	if (this->dock != dock) {
		this->dock = dock;
		this->Layout();
	}
}

awuiControl::DockStyle awuiControl::GetDock() {
	return this->dock;
}

awuiControl * awuiControl::GetParent() {
	return this->parent;
}

void awuiControl::SetParent(awuiControl * parent) {
	this->parent = parent;
}

void awuiControl::Layout() {
	int x1 = 0;
	int y1 = 0;
	int x2 = this->GetWidth() - 1;
	int y2 = this->GetHeight() - 1;
	int margin = 1;
	
	for (int i = 0; i < this->GetControls()->GetCount(); i++) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);
		switch (control->GetDock()) {
			case awuiControl::Fill:
				control->SetBounds(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
				break;
			case awuiControl::Left:
				control->SetBounds(x1, y1, control->GetWidth(), y2 - y1 + 1);
				x1 += (control->GetWidth() + margin);
				break;
			case awuiControl::Right:
				control->SetBounds(x2 - control->GetWidth() + 1, y1, control->GetWidth(), y2 - y1 + 1);
				x2 -= (control->GetWidth() + margin);
				break;
			case awuiControl::Top:
				control->SetBounds(x1, y1, x2 - x1 + 1, control->GetHeight());
				y1 += (control->GetHeight() + margin);
				break;
			case awuiControl::Bottom:
				control->SetBounds(x1, y2 - control->GetHeight() + 1, x2 - x1 + 1, control->GetHeight());
				y2 -= (control->GetHeight() + margin);
				break;
		}
	}
}

void awuiControl::Refresh() {
	if (this->GetParent())
		this->GetParent()->Refresh();

	this->needRefresh = 1;
}

void awuiControl::OnPaintPre(int x, int y, int width, int height, awuiGL * gl) {
	awuiRectangle rect2;
	rect2.SetX(x);
	rect2.SetY(height - y - this->GetHeight());
	rect2.SetWidth(this->GetWidth());
	rect2.SetHeight(this->GetHeight());

	gl->SetClipping(rect2);
	gl->SetClipping();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-x, width-x, height - y - 1, -y, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(this->backColor->GetR() / 255.0f, this->backColor->GetG() / 255.0f, this->backColor->GetB() / 255.0f, this->backColor->GetA() / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT);

/*
	glColor3f(1.0f, 0.0f, 0.0f);
	//glBegin(GL_POLYGON);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(this->GetWidth() - 1, 0.0f);
	glVertex2f(this->GetWidth() - 1, this->GetHeight() - 1);
	glVertex2f(0.0f, this->GetHeight() - 1);
	glEnd();
*/

	this->OnPaint(NULL);

	for (int i = 0; i < this->GetControls()->GetCount(); i++) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);

		awuiGL gl2;
		gl2.SetClippingBase(gl->GetClippingResult());
		control->OnPaintPre(x + control->GetLeft(), y + control->GetTop(), width, height, &gl2);
	}
}

#include <iostream>

void awuiControl::OnMouseDownPre(MouseButtons::Buttons button, int buttons) {
	int x = this->mouseEventArgs->GetX();
	int y = this->mouseEventArgs->GetY();

	for (int i = this->GetControls()->GetCount() - 1; i >= 0; i--) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);

		if (this->mouseControl != NULL) {
			if (this->mouseControl == control) {
				control->OnMouseDownPre(button, buttons);
				return;
			}
		} else {
			if ((control->GetLeft() <= x) && (x <= control->GetRight()) && (control->GetTop() <= y) && (y <= control->GetBottom())) {
				this->mouseControl = control;
				control->OnMouseDownPre(button, buttons);
				return;
			}
		}
	}
	
	this->mouseEventArgs->SetButton(button);
	this->OnMouseDown(this->mouseEventArgs);

//	std::cout << "Down: " << this->mouseEventArgs->GetX() << "x" << this->mouseEventArgs->GetY() << "  " << this->mouseEventArgs->GetButton() << "    " << this->GetName() << std::endl;
}

void awuiControl::OnMouseMovePre(int x, int y, int buttons) {
	this->mouseEventArgs->SetLocation(x, y);

	for (int i = this->GetControls()->GetCount() - 1; i >= 0; i--) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);

		int find = 0;
		
		if (this->mouseControl != NULL) {
			if (this->mouseControl == control)
				find = 1;
		} else {
			if ((control->GetLeft() <= x) && (x <= control->GetRight()) && (control->GetTop() <= y) && (y <= control->GetBottom()))
				find = 1;
		}

		if (find) {
			if (this->mouseControlOver != control) {
				if (this->mouseControlOver != NULL)
					this->mouseControlOver->OnMouseLeave();

				this->mouseControlOver = control;
				control->OnMouseEnter();
			}
			control->OnMouseMovePre(x - control->GetLeft(), y - control->GetTop(), buttons);
			return;
		}
	}
	
	this->mouseEventArgs->SetButton(buttons);
	this->OnMouseMove(this->mouseEventArgs);
	this->Refresh();

//	std::cout << "Move: " << this->mouseEventArgs->GetX() << "x" << this->mouseEventArgs->GetY() << "  " << this->mouseEventArgs->GetButton() << "    " << this->GetName() << std::endl;
}

void awuiControl::OnMouseUpPre(MouseButtons::Buttons button, int buttons) {
	int x = this->mouseEventArgs->GetX();
	int y = this->mouseEventArgs->GetY();

	for (int i = this->GetControls()->GetCount() - 1; i >= 0; i--) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);

		if (this->mouseControl != NULL) {
			if (this->mouseControl == control) {
				this->mouseControl = NULL;
				control->OnMouseUpPre(button, buttons);
				return;
			}
		} else {
			if ((control->GetLeft() <= x) && (x <= control->GetRight()) && (control->GetTop() <= y) && (y <= control->GetBottom())) {
				control->OnMouseUpPre(button, buttons);
				return;
			}
		}
	}
	
	this->mouseEventArgs->SetButton(button);
	this->OnMouseUp(this->mouseEventArgs);

//	std::cout << "Up: " << this->mouseEventArgs->GetX() << "x" << this->mouseEventArgs->GetY() << "  " << this->mouseEventArgs->GetButton() << "    " << this->GetName() << std::endl;
}

void awuiControl::OnMouseLeave() {
//	std::cout << "OnMouseLeave: " << this->GetName() << std::endl;
}

void awuiControl::OnMouseEnter() {
//	std::cout << "OnMouseEnter: " << this->GetName() << std::endl;
}


void awuiControl::SetName(const std::string& str) {
	this->name.assign(str);
}

const std::string& awuiControl::GetName() {
	return this->name;
}