// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControl.h"

#include "awuiBitmap.h"
#include "awuiColor.h"
#include "awuiControlCollection.h"
#include "awuiGraphics.h"
#include "awuiMouseEventArgs.h"
#include "awuiPen.h"

awuiControl::awuiControl() {
	this->controls = new awuiControlCollection(this);
	this->mouseEventArgs = new awuiMouseEventArgs();
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
	if (this->bitmap != NULL)
		delete this->bitmap;

	this->bitmap = new awuiBitmap(this->GetWidth(), this->GetHeight());
	
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
	int x2 = this->GetWidth();
	int y2 = this->GetHeight();
	
	for (int i = 0; i < this->GetControls()->GetCount(); i++) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);
		switch (control->GetDock()) {
			case awuiControl::Fill:
				control->SetBounds(x1, y1, x2 - x1, y2 - y1);
				break;
			case awuiControl::Left:
				control->SetBounds(x1, y1, control->GetWidth(), y2 - y1);
				x1 += control->GetWidth();
				break;
			case awuiControl::Right:
				control->SetBounds(x2 - control->GetWidth(), y1, control->GetWidth(), y2 - y1);
				x2 -= control->GetWidth();
				break;
			case awuiControl::Top:
				control->SetBounds(x1, y1, x2 - x1, control->GetHeight());
				y1 += control->GetHeight();
				break;
			case awuiControl::Bottom:
				control->SetBounds(x1, y2 - control->GetHeight(), x2 - x1, control->GetHeight());
				y2 -= control->GetHeight();
				break;
		}
	}
}

void awuiControl::OnPaintPre(awuiGraphics * g) {
	awuiColor * color = awuiColor::FromArgb(0, 0, 0);
	awuiPen * pen = new awuiPen(color);

	g->FillRectangle(this->backColor, 0.0f, 0.0f, (float)this->GetWidth(), (float)this->GetHeight());

	for (int i = 0; i < this->GetControls()->GetCount(); i++) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);

		awuiGraphics * g2 = awuiGraphics::FromImage(control->bitmap);
		g2->FillRectangle(control->backColor, 0.0f, 0.0f, (float)control->GetWidth(), (float)control->GetHeight());
		control->OnPaintPre(g2);
		g->DrawImage(control->bitmap, (float)control->GetLeft(), (float)control->GetTop());
		delete g2;
	}

	this->OnPaint(g);
	g->DrawLine(pen, (float)this->mouseEventArgs->GetX() - 5.0f, (float)this->mouseEventArgs->GetY(), (float)this->mouseEventArgs->GetX() + 5.0f, (float)this->mouseEventArgs->GetY());
	g->DrawLine(pen, (float)this->mouseEventArgs->GetX(), (float)this->mouseEventArgs->GetY() - 5.0f, (float)this->mouseEventArgs->GetX(), (float)this->mouseEventArgs->GetY() + 5.0f);

	delete pen;
	delete color;
}

#include <iostream>

void awuiControl::OnMouseMovePre(int x, int y, int buttons) {
	this->mouseEventArgs->SetLocation(x, y);

	for (int i = this->GetControls()->GetCount() - 1; i >= 0; i--) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);

		if ((control->GetLeft() <= x) && (x <= control->GetRight()) && (control->GetTop() <= y) && (y <= control->GetBottom())) {
			control->OnMouseMovePre(x - control->GetLeft(), y - control->GetTop(), buttons);
			return;
		}
	}
	
	this->mouseEventArgs->SetButton(buttons);
	this->OnMouseMove(this->mouseEventArgs);

	std::cout << "Move: " << this->mouseEventArgs->GetX() << "x" << this->mouseEventArgs->GetY() << "  " << this->mouseEventArgs->GetButton() << "    " << this->GetName() << std::endl;
}

void awuiControl::OnMouseUpPre(MouseButtons::Buttons button, int buttons) {
	int x = this->mouseEventArgs->GetX();
	int y = this->mouseEventArgs->GetY();

	for (int i = this->GetControls()->GetCount() - 1; i >= 0; i--) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);

		if ((control->GetLeft() <= x) && (x <= control->GetRight()) && (control->GetTop() <= y) && (y <= control->GetBottom())) {
			control->OnMouseUpPre(button, buttons);
			return;
		}
	}
	
	this->mouseEventArgs->SetButton(button);
	this->OnMouseUp(this->mouseEventArgs);

	std::cout << "Up: " << this->mouseEventArgs->GetX() << "x" << this->mouseEventArgs->GetY() << "  " << this->mouseEventArgs->GetButton() << "    " << this->GetName() << std::endl;
}

void awuiControl::OnMouseDownPre(MouseButtons::Buttons button, int buttons) {
	int x = this->mouseEventArgs->GetX();
	int y = this->mouseEventArgs->GetY();

	for (int i = this->GetControls()->GetCount() - 1; i >= 0; i--) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);

		if ((control->GetLeft() <= x) && (x <= control->GetRight()) && (control->GetTop() <= y) && (y <= control->GetBottom())) {
			control->OnMouseDownPre(button, buttons);
			return;
		}
	}
	
	this->mouseEventArgs->SetButton(button);
	this->mouseControl = this;
	this->OnMouseDown(this->mouseEventArgs);

	std::cout << "Down: " << this->mouseEventArgs->GetX() << "x" << this->mouseEventArgs->GetY() << "  " << this->mouseEventArgs->GetButton() << "    " << this->GetName() << std::endl;
}

void awuiControl::SetName(const std::string& str) {
	this->name.assign(str);
}

const std::string& awuiControl::GetName() {
	return this->name;
}