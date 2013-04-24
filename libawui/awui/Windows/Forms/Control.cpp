// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Control.h"

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Pen.h>
#include <awui/Drawing/Rectangle.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <SDL_opengl.h>

#include <iostream>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

Control::Control() {
	this->bounds = Rectangle(0, 0, 100, 100);
	this->controls = new ControlCollection(this);
	this->mouseEventArgs = new MouseEventArgs();
	this->mouseControl = NULL;
	this->parent = NULL;
	this->needRefresh = 1;
	this->dock = DockStyle::None;
	this->backColor = Color::FromArgb(226, 226, 226);
	this->OnResizePre();
}

Control::~Control() {
	delete this->mouseEventArgs;

	for (int i = 0; i < this->controls->GetCount(); i++)
		delete ((Control *)this->controls->Get(i));

	this->controls->Clear();
	delete this->controls;
}

int Control::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Control)
		return 1;

	return Object::IsClass(objectClass);
}

void Control::SetTop(int y) {
	this->SetLocation(this->bounds.GetX(), y);
}

void Control::SetLeft(int x) {
	this->SetLocation(x, this->bounds.GetY());
}

void Control::SetLocation(int x, int y) {
	this->SetBounds(x, y, this->bounds.GetWidth(), this->bounds.GetHeight());
}

void Control::SetWidth(int width) {
	this->SetSize(width, this->bounds.GetHeight());
}

void Control::SetHeight(int height) {
	this->SetSize(this->bounds.GetWidth(), height);
}

void Control::SetSize(int width, int height) {
	this->SetBounds(this->bounds.GetX(), this->bounds.GetY(), width, height);
}

void Control::SetSize(const Size size) {
	this->SetSize(size.GetWidth(), size.GetHeight());
}

void Control::SetBounds(int x, int y, int width, int height) {
	if (width < this->minimumSize.GetWidth())
		width = this->minimumSize.GetWidth();

	if (height < this->minimumSize.GetHeight())
		height = this->minimumSize.GetHeight();

	if ((this->bounds.GetX() == x) && (this->bounds.GetY() == y) && (this->bounds.GetWidth() == width) && (this->bounds.GetHeight() == height))
		return;

	this->bounds = Rectangle(x, y, width, height);
	this->Refresh();
	this->OnResizePre();
}

int Control::GetTop() const {
	return this->bounds.GetTop();
}

int Control::GetLeft() const {
	return this->bounds.GetLeft();
}

int Control::GetRight() const {
	return this->bounds.GetRight();
}

int Control::GetBottom() const {
	return this->bounds.GetBottom();
}

const Point Control::GetLocation() const {
	return this->bounds.GetLocation();
}

int Control::GetWidth() const {
	return this->bounds.GetWidth();
}

int Control::GetHeight() const {
	return this->bounds.GetHeight();
}

const Size Control::GetSize() const {
	return this->bounds.GetSize();
}

const Rectangle Control::GetBounds() const {
	return this->bounds;
}

ControlCollection* Control::GetControls() const {
	return this->controls;
}

void Control::OnResizePre() {
	this->OnResize();
	this->Layout();
}

void Control::SetBackColor(const awui::Drawing::Color color) {
	this->backColor = color;
}

awui::Drawing::Color Control::GetBackColor() {
	return this->backColor;
}

void Control::SetDock(DockStyle::Enum dock) {
	if (this->dock != dock) {
		this->dock = dock;
		this->Layout();
	}
}

Control::DockStyle::Enum Control::GetDock() const {
	return this->dock;
}

const Size Control::GetMinimumSize() const {
	return this->minimumSize;
}

void Control::SetMinimumSize(Size size) {
	this->minimumSize = size;
	this->SetSize(this->GetSize());
}

Control * Control::GetParent() const {
	return this->parent;
}

void Control::SetParent(Control * parent) {
	this->parent = parent;
}

void Control::Layout() {
	int x1 = 0;
	int y1 = 0;
	int x2 = this->GetWidth() - 1;
	int y2 = this->GetHeight() - 1;
	int margin = 1;

	for (int i = 0; i < this->GetControls()->GetCount(); i++) {
		Control * control = (Control *)this->GetControls()->Get(i);
		switch (control->GetDock()) {
			case DockStyle::None:
				break;
			case DockStyle::Fill:
				control->SetBounds(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
				break;
			case DockStyle::Left:
				control->SetBounds(x1, y1, control->GetWidth(), y2 - y1 + 1);
				x1 += (control->GetWidth() + margin);
				break;
			case DockStyle::Right:
				control->SetBounds(x2 - control->GetWidth() + 1, y1, control->GetWidth(), y2 - y1 + 1);
				x2 -= (control->GetWidth() + margin);
				break;
			case DockStyle::Top:
				control->SetBounds(x1, y1, x2 - x1 + 1, control->GetHeight());
				y1 += (control->GetHeight() + margin);
				break;
			case DockStyle::Bottom:
				control->SetBounds(x1, y2 - control->GetHeight() + 1, x2 - x1 + 1, control->GetHeight());
				y2 -= (control->GetHeight() + margin);
				break;
		}
	}
}

void Control::Refresh() {
	if (this->GetParent())
		this->GetParent()->Refresh();

	this->needRefresh = 1;
}

void Control::OnPaintPre(int x, int y, int width, int height, GL * gl) {
	Rectangle rect2;
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
		Control * control = (Control *)this->GetControls()->Get(i);

		GL gl2;
		gl2.SetClippingBase(gl->GetClippingResult());
		control->OnPaintPre(x + control->GetLeft(), y + control->GetTop(), width, height, &gl2);
	}
}

void Control::OnMouseDownPre(int x, int y, MouseButtons::Enum button, int buttons) {
	this->mouseEventArgs->SetLocation(x, y);

	for (int i = this->GetControls()->GetCount() - 1; i >= 0; i--) {
		Control * control = (Control *)this->GetControls()->Get(i);

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

void Control::OnMouseMovePre(int x, int y, int buttons) {
	this->mouseEventArgs->SetLocation(x, y);

	for (int i = this->GetControls()->GetCount() - 1; i >= 0; i--) {
		Control * control = (Control *)this->GetControls()->Get(i);

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

void Control::ChangeControlOnMouseOver(Control * control) {
	if (this->GetParent()) {
		this->GetParent()->ChangeControlOnMouseOver(control);
		return;
	}

	if (this->IsClass(Classes::Form)) {
		if (((Form *) this)->mouseControlOver != control) {
			if (((Form *) this)->mouseControlOver != NULL)
				((Form *) this)->mouseControlOver->OnMouseLeave();

			((Form *) this)->mouseControlOver = control;
			control->OnMouseEnter();
		}
	}
}

void Control::OnMouseUpPre(MouseButtons::Enum button, int buttons) {
	int x = this->mouseEventArgs->GetX();
	int y = this->mouseEventArgs->GetY();

	for (int i = this->GetControls()->GetCount() - 1; i >= 0; i--) {
		Control * control = (Control *)this->GetControls()->Get(i);

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

void Control::OnMouseLeave() {
//	std::cout << "OnMouseLeave: " << this->GetName() << std::endl;
}

void Control::OnMouseEnter() {
//	std::cout << "OnMouseEnter: " << this->GetName() << std::endl;
}

void Control::SetName(const String str) {
	this->name = str;
}

const awui::String Control::GetName() {
	return this->name;
}

void Control::OnTickPre() {
	this->OnTick();

	for (int i = 0; i<this->GetControls()->GetCount(); i++) {
		Control * control = (Control *)this->GetControls()->Get(i);
		control->OnTickPre();
	}
}
