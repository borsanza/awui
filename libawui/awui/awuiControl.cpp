// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControl.h"

#include "awuiColor.h"
#include "awuiControlCollection.h"
#include "awuiForm.h"
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
	this->bounds = awuiRectangle(0, 0, 100, 100);
	this->controls = new awuiControlCollection(this);
	this->mouseEventArgs = new awuiMouseEventArgs();
	this->mouseControl = NULL;
	this->parent = NULL;
	this->needRefresh = 1;
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
}

int awuiControl::IsClass(awuiObject::awuiClasses objectClass) const {
	if (objectClass == awuiObject::Control)
		return 1;

	return awuiObject::IsClass(objectClass);
}

void awuiControl::SetTop(int y) {
	this->SetLocation(this->bounds.GetX(), y);
}

void awuiControl::SetLeft(int x) {
	this->SetLocation(x, this->bounds.GetY());
}

void awuiControl::SetLocation(int x, int y) {
	this->SetBounds(x, y, this->bounds.GetWidth(), this->bounds.GetHeight());
}

void awuiControl::SetWidth(int width) {
	this->SetSize(width, this->bounds.GetHeight());
}

void awuiControl::SetHeight(int height) {
	this->SetSize(this->bounds.GetWidth(), height);
}

void awuiControl::SetSize(int width, int height) {
	this->SetBounds(this->bounds.GetX(), this->bounds.GetY(), width, height);
}

void awuiControl::SetSize(const awuiSize size) {
	this->SetSize(size.GetWidth(), size.GetHeight());
}

void awuiControl::SetBounds(int x, int y, int width, int height) {
	if (width < this->minimumSize.GetWidth())
		width = this->minimumSize.GetWidth();

	if (height < this->minimumSize.GetHeight())
		height = this->minimumSize.GetHeight();

	if ((this->bounds.GetX() == x) && (this->bounds.GetY() == y) && (this->bounds.GetWidth() == width) && (this->bounds.GetHeight() == height))
		return;

	this->bounds = awuiRectangle(x, y, width, height);
	this->Refresh();
	this->OnResizePre();
}

int awuiControl::GetTop() const {
	return this->bounds.GetTop();
}

int awuiControl::GetLeft() const {
	return this->bounds.GetLeft();
}

int awuiControl::GetRight() const {
	return this->bounds.GetRight();
}

int awuiControl::GetBottom() const {
	return this->bounds.GetBottom();
}

const awuiPoint awuiControl::GetLocation() const {
	return this->bounds.GetLocation();
}

int awuiControl::GetWidth() const {
	return this->bounds.GetWidth();
}

int awuiControl::GetHeight() const {
	return this->bounds.GetHeight();
}

const awuiSize awuiControl::GetSize() const {
	return this->bounds.GetSize();
}

const awuiRectangle awuiControl::GetBounds() const {
	return this->bounds;
}

awuiControlCollection* awuiControl::GetControls() const {
	return this->controls;
}

void awuiControl::OnResizePre() {
	this->OnResize();
	this->Layout();
}

void awuiControl::SetBackColor(const awuiColor color) {
	this->backColor = color;
}

awuiColor awuiControl::GetBackColor() {
	return this->backColor;
}

void awuiControl::SetDock(awuiControl::DockStyle dock) {
	if (this->dock != dock) {
		this->dock = dock;
		this->Layout();
	}
}

awuiControl::DockStyle awuiControl::GetDock() const {
	return this->dock;
}


const awuiSize awuiControl::GetMinimumSize() const {
	return this->minimumSize;
}

void awuiControl::SetMinimumSize(awuiSize size) {
	this->minimumSize = size;
	this->SetSize(this->GetSize());
}

awuiControl * awuiControl::GetParent() const {
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
	glOrtho(-x - 1, width - x, height - y, -y, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(this->backColor.GetR() / 255.0f, this->backColor.GetG() / 255.0f, this->backColor.GetB() / 255.0f, this->backColor.GetA() / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	this->OnPaint(NULL);

	for (int i = 0; i < this->GetControls()->GetCount(); i++) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);

		awuiGL gl2;
		gl2.SetClippingBase(gl->GetClippingResult());
		control->OnPaintPre(x + control->GetLeft(), y + control->GetTop(), width, height, &gl2);
	}
}

#include <iostream>

void awuiControl::OnMouseDownPre(int x, int y, MouseButtons::Buttons button, int buttons) {
	this->mouseEventArgs->SetLocation(x, y);

	for (int i = this->GetControls()->GetCount() - 1; i >= 0; i--) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);

		if (this->mouseControl != NULL) {
			if (this->mouseControl == control) {
				control->OnMouseDownPre(x - control->GetLeft(), y - control->GetTop(), button, buttons);
				return;
			}
		} else {
			if ((control->GetLeft() <= x) && (x <= control->GetRight()) && (control->GetTop() <= y) && (y <= control->GetBottom())) {
				this->mouseControl = control;
				this->ChangeControlOnMouseOver(control);
				control->OnMouseDownPre(x - control->GetLeft(), y - control->GetTop(), button, buttons);
				return;
			}
		}
	}

	this->mouseEventArgs->SetButton(button);
	this->OnMouseDown(this->mouseEventArgs);

//	std::cout << "Down: " << this->mouseEventArgs->GetX() << "x" << this->mouseEventArgs->GetY() << "   " << this->mouseEventArgs->GetButton() << "   " << this->GetName() << std::endl;
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
			this->ChangeControlOnMouseOver(control);

			control->OnMouseMovePre(x - control->GetLeft(), y - control->GetTop(), buttons);
			return;
		}
	}

	this->mouseEventArgs->SetButton(buttons);
	this->OnMouseMove(this->mouseEventArgs);
	this->Refresh();

//	std::cout << "Move: " << this->mouseEventArgs->GetX() << "x" << this->mouseEventArgs->GetY() << "   " << this->mouseEventArgs->GetButton() << "   " << this->GetName() << std::endl;
}

void awuiControl::ChangeControlOnMouseOver(awuiControl * control) {
	if (this->GetParent()) {
		this->GetParent()->ChangeControlOnMouseOver(control);
		return;
	}

	if (this->IsClass(awuiObject::Form)) {
		if (((awuiForm *) this)->mouseControlOver != control) {
			if (((awuiForm *) this)->mouseControlOver != NULL)
				((awuiForm *) this)->mouseControlOver->OnMouseLeave();

			((awuiForm *) this)->mouseControlOver = control;
			control->OnMouseEnter();
		}
	}
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

//	std::cout << "Up: " << this->mouseEventArgs->GetX() << "x" << this->mouseEventArgs->GetY() << "   " << this->mouseEventArgs->GetButton() << "   " << this->GetName() << std::endl;
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

void awuiControl::OnTickPre() {
	this->OnTick();

	for (int i = 0; i<this->GetControls()->GetCount(); i++) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);
		control->OnTickPre();
	}
}
