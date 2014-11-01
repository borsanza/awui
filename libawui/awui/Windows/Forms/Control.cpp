/*
 * awui/Windows/Forms/Control.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Control.h"

#include <awui/Collections/ArrayList.h>
#include <awui/Console.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Pen.h>
#include <awui/Drawing/Rectangle.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <awui/Windows/Forms/Statistics/Stats.h>
#include <SDL_opengl.h>

#include <iostream>

using namespace awui::Collections;
using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Statistics;

Control::Control() {
	this->tabStop = false;
	this->bounds = Rectangle(0, 0, 100, 100);
	this->controls = new ControlCollection(this);
	this->mouseEventArgs = new MouseEventArgs();
	this->mouseControl = NULL;
	this->parent = NULL;
	this->focused = NULL;
	this->needRefresh = 1;
	this->dock = DockStyle::None;
	this->backColor = Color::FromArgb(0, 0, 0, 0);
	this->foreColor = Color::FromArgb(255, 255, 255);
	this->OnResizePre();
	this->font = new Font("sans-serif", 12);
	this->scissorEnabled = true;
}

Control::~Control() {
	delete this->font;
	delete this->mouseEventArgs;

	while (this->controls->GetCount() > 0) {
		Control * control = (Control *) this->controls->Get(0);
		delete control;
		this->controls->RemoveAt(0);
	}

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

int Control::GetAbsoluteTop() const {
	Control * parent = GetParent();
	int pos = 0;
	if (parent)
		pos = parent->GetAbsoluteTop();

	return this->GetTop() + pos;
}


int Control::GetLeft() const {
	return this->bounds.GetLeft();
}

int Control::GetAbsoluteLeft() const {
	Control * parent = GetParent();
	int pos = 0;
	if (parent)
		pos = parent->GetAbsoluteLeft();

	return this->GetLeft() + pos;
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

void Control::SetForeColor(const awui::Drawing::Color color) {
	this->foreColor = color;
}

awui::Drawing::Color Control::GetForeColor() {
	return this->foreColor;
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

int Control::OnPaintPre(int x, int y, int width, int height, GL * gl) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-x, width - x, height - y, -y, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Rectangle rect2;
	rect2.SetX(x);
	rect2.SetY(height - y - this->GetHeight());
	rect2.SetWidth(this->GetWidth());
	rect2.SetHeight(this->GetHeight());
	gl->SetClipping(rect2);

	bool disableScissor = false;
	if (this->scissorEnabled) {
		disableScissor = true;
		glEnable(GL_SCISSOR_TEST);
		gl->SetClipping();
	}

	switch (this->backColor.GetA()) {
		case 255:
			if ((width == this->GetWidth()) && (height == this->GetHeight())) {
				glClearColor(this->backColor.GetR() / 255.0f, this->backColor.GetG() / 255.0f, this->backColor.GetB() / 255.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
			} else {
				glColor3f(this->backColor.GetR() / 255.0f, this->backColor.GetG() / 255.0f, this->backColor.GetB() / 255.0f);
				GL::FillRectangle(0, 0, this->GetWidth(), this->GetHeight());
			}
			break;
		case 0:
			break;
		default:
			glColor4f(this->backColor.GetR() / 255.0f, this->backColor.GetG() / 255.0f, this->backColor.GetB() / 255.0f, this->backColor.GetA() / 255.0f);
			GL::FillRectangle(0, 0, this->GetWidth(), this->GetHeight());
			break;
	}

	int r = 0;

	Rectangle cr = gl->GetClippingResult();
	int isVisible = (cr.GetWidth() > 0) && (cr.GetHeight() > 0);
	if (isVisible) {
		r++;
		this->OnPaint(NULL);
	}

	if (disableScissor)
		glDisable(GL_SCISSOR_TEST);

	if (isVisible) {
		for (int i = 0; i < this->GetControls()->GetCount(); i++) {
			Control * control = (Control *)this->GetControls()->Get(i);

			GL gl2;
			gl2.SetClippingBase(gl->GetClippingResult());
			r += control->OnPaintPre(x + control->GetLeft(), y + control->GetTop(), width, height, &gl2);
		}
	}

	return r;
}

float Interpolate(float from, int to, float percent) {
	if (awui::Math::Round(from) == to)
		return from;

	return from + ((to - from) * percent);
}

// Lo usamos para dibujar el skin
void Control::OnPaint(OpenGL::GL * gl) {
	static float lastx1, lasty1, lastwidth, lastheight;
	static Control * lastParent = NULL;
	Control * focused = Form::GetControlSelected();

	for (int i = 0; i < this->GetControls()->GetCount(); i++) {
		Control * control = (Control *)this->GetControls()->Get(i);

		if (focused == control) {
			int x1, y1, x2, y2;
			Bitmap * bitmap = Form::GetSelectedBitmap();
			float percent = 0.4f;

			// Esto solo lo hago porque en la inicializacion del programa
			// no habia nada seleccionado y hay que actualizar esta variable
			// estatica
			if (lastParent == NULL)
				Form::SetControlSelected(control);

			if (lastParent != bitmap->GetParent()) {
				lastParent = bitmap->GetParent();
				percent = 1.0f;
			}

			bitmap->GetFixedMargins(&x1, &y1, &x2, &y2);

			int width = control->GetWidth() + x1 + x2;
			int height = control->GetHeight() + y1 + y2;

			lastwidth = Interpolate(lastwidth, width, percent);
			lastheight = Interpolate(lastheight, height, percent);
			width = Math::Round(lastwidth);
			height = Math::Round(lastheight);

			bitmap->SetSize(width, height);
			int x = control->GetLeft() - x1;
			int y = control->GetTop() - y1;

			lastx1 = Interpolate(lastx1, x, percent);
			lasty1 = Interpolate(lasty1, y, percent);
			x = Math::Round(lastx1);
			y = Math::Round(lasty1);

			glTranslatef(x, y, 0);
			bitmap->OnPaint(NULL);
			glTranslatef(-x, -y, 0);
		}
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
		if (!control->IsVisible())
			continue;

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

bool Control::IsVisible() const {
	bool isVisible = true;

	if (this->GetParent()) {
		Rectangle pr = this->GetParent()->GetBounds();
		pr.SetLocation(0, 0);
		Rectangle cr = Rectangle::Intersect(this->bounds, pr);
		isVisible = (cr.GetWidth() > 0) && (cr.GetHeight() > 0);
	}

	return isVisible;
}

void Control::OnTickPre() {
	if (this->IsVisible()) {
		this->OnTick();

		for (int i = 0; i<this->GetControls()->GetCount(); i++) {
			Control * control = (Control *)this->GetControls()->Get(i);
			control->OnTickPre();
		}
	}
}

awui::Drawing::Font * Control::GetFont() {
	return this->font;
}

void Control::SetFont(const Drawing::Font * font) {
	*this->font = *font;
}

void Control::SetScissorEnabled(bool mode) {
	this->scissorEnabled = mode;
}

bool Control::GetScissorEnabled() {
	return this->scissorEnabled;
}

bool Control::GetTabStop() {
	return this->tabStop;
}

void Control::SetTabStop(bool tabStop) {
	this->tabStop = tabStop;
}

void Control::OnRemoteKeyPressPre(RemoteButtons::Enum button) {
	bool mustStop = this->OnRemoteKeyPress(button);
	if (mustStop)
		return;

	if (!this->focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (this->focused)
		focused->OnRemoteKeyPressPre(button);
}

void Control::OnRemoteKeyUpPre(RemoteButtons::Enum button) {
	bool mustStop = this->OnRemoteKeyUp(button);
	if (mustStop)
		return;

	if (!this->focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (this->focused)
		focused->OnRemoteKeyUpPre(button);
}

void Control::OnKeyPressPre(Keys::Enum key) {
	bool mustStop = this->OnKeyPress(key);
	if (mustStop)
		return;

	if (!this->focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (this->focused)
		focused->OnKeyPressPre(key);
}

void Control::OnKeyUpPre(Keys::Enum key) {
	bool mustStop = this->OnKeyUp(key);
	if (mustStop)
		return;

	if (!this->focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (this->focused)
		focused->OnKeyUpPre(key);
}

void Control::SetFocus(bool selectControl) {
	Control * parent = this->GetParent();
	if (parent) {
		if (selectControl)
			Form::SetControlSelected(this);
		parent->focused = this;
		parent->SetFocus(false);
	}
}

Control * Control::GetTopParent() {
	Control * parent = GetParent();
	if (parent)
		return parent->GetTopParent();

	return this;
}

bool Control::OnRemoteKeyUp(RemoteButtons::Enum button) {
	return false;
}

bool Control::OnKeyUp(Keys::Enum button) {
	return false;
}

bool Control::OnRemoteKeyPress(RemoteButtons::Enum button) {
	if (Form::GetControlSelected() == this) {
		Point pCenter(Math::Round((this->GetWidth() / 2.0f) + this->GetAbsoluteLeft()), Math::Round((this->GetHeight() / 2.0f) + this->GetAbsoluteTop()));
		Point p1;
		Point p2;
		int distance = 3000;

		switch (button) {
			case RemoteButtons::Up:
				pCenter.SetY(this->GetAbsoluteTop());
				p1.SetX(pCenter.GetX() - distance);
				p1.SetY(pCenter.GetY() - distance);
				p2.SetX(pCenter.GetX() + distance);
				p2.SetY(pCenter.GetY() - distance);
				break;
			case RemoteButtons::Right:
				pCenter.SetX(this->GetAbsoluteLeft() + this->GetWidth());
				p1.SetX(pCenter.GetX() + distance);
				p1.SetY(pCenter.GetY() - distance);
				p2.SetX(pCenter.GetX() + distance);
				p2.SetY(pCenter.GetY() + distance);
				break;
			case RemoteButtons::Down:
				pCenter.SetY(this->GetAbsoluteTop() + this->GetHeight());
				p1.SetX(pCenter.GetX() - distance);
				p1.SetY(pCenter.GetY() + distance);
				p2.SetX(pCenter.GetX() + distance);
				p2.SetY(pCenter.GetY() + distance);
				break;
			case RemoteButtons::Left:
				pCenter.SetX(this->GetAbsoluteLeft());
				p1.SetX(pCenter.GetX() - distance);
				p1.SetY(pCenter.GetY() - distance);
				p2.SetX(pCenter.GetX() - distance);
				p2.SetY(pCenter.GetY() + distance);
				break;
			default:
				break;
		}

		switch (button) {
			case RemoteButtons::Up:
			case RemoteButtons::Right:
			case RemoteButtons::Down:
			case RemoteButtons::Left:
				{
					Control * control = this->GetTopParent();
					if (control) {
						control = control->GetNextControl(this, &pCenter, &p1, &p2);
						if (control)
							Form::SetControlSelected(control);
					}
				}
				break;
			case RemoteButtons::Play:
				Console::WriteLine("Play");
				break;
			case RemoteButtons::Ok:
				Console::WriteLine("Ok");
				break;
			case RemoteButtons::Menu:
				Console::WriteLine("Menu");
				break;
			default:
				break;
		}
	}

	return false;
}

bool Control::OnKeyPress(Keys::Enum key) {
	if (Form::GetControlSelected() == this) {
	}

	return false;
}

void Control::GetControlsSelectables(ArrayList * list) {
	for (int i = 0; i<this->GetControls()->GetCount(); i++) {
		Control * control = (Control *)this->GetControls()->Get(i);
		control->GetControlsSelectables(list);
	}

	if (this->GetTabStop())
		list->Add(this);
}

#define PARTS 4.0f

Control * Control::GetNextControl(Control * ommitControl, Point * pCenter, Point * p1, Point * p2) {
	ArrayList list;
	this->GetControlsSelectables(&list);

	Control * selected = NULL;
	float distance = 3000;

	for (int i = 0; i<list.GetCount(); i++) {
		Control * control = (Control *)list.Get(i);
		if (control == ommitControl)
			continue;

		for (int j2=0; j2<=PARTS; j2++) {
			for (int j3=0; j3<=PARTS; j3++) {
				int x = Math::Round(control->GetAbsoluteLeft() + (control->GetWidth() * j2 / PARTS));
				int y = Math::Round(control->GetAbsoluteTop() + (control->GetHeight() * j3 / PARTS));
				Point pControlCenter(x, y);
				if (pControlCenter.InTriangle(pCenter, p1, p2)) {
					float dist = Math::Sqrt(Math::Pow((float)pCenter->GetX() - pControlCenter.GetX(), 2.0f)
																+ Math::Pow((float)pCenter->GetY() - pControlCenter.GetY(), 2.0f));
					if (dist < distance) {
						distance = dist;
						selected = control;
					}
				}
			}
		}
	}

	return selected;
}
