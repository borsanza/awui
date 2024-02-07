/**
 * awui/Windows/Forms/Control.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Control.h"

#include <awui/Drawing/Font.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

Control::Control() {
	this->refreshed = 0;
	this->_lastWidth = 0;
	this->_lastHeight = 0;
	this->_lastX = 0;
	this->_lastY = 0;
	this->_drawShadow = true;
	this->tabStop = false;
	this->bounds = Drawing::Rectangle(0, 0, 100, 100);
	this->boundsTo = this->bounds;
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
	this->_preventChangeControl = false;
	this->_visible = true;
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

	this->bounds = Drawing::Rectangle(x, y, width, height);
	this->boundsTo = this->bounds;
	this->_lastWidth = width;
	this->_lastHeight = height;
	this->_lastX = x;
	this->_lastY = y;

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

int Control::GetAbsoluteBottom() const {
	return this->GetAbsoluteTop() + this->GetHeight() - 1;
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

int Control::GetAbsoluteRight() const {
	return this->GetAbsoluteLeft() + this->GetWidth() - 1;
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

const awui::Drawing::Rectangle Control::GetBounds() const {
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
	int margin = 0;

	for (int i = 0; i < this->GetControls()->GetCount(); i++) {
		Control * control = (Control *)this->GetControls()->Get(i);
		if (!control->IsVisible())
			continue;

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

int Control::OnPaintPre(int x, int y, int width, int height, GL * gl, bool first) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-x, width - x, height - y, -y, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Drawing::Rectangle rect2;
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

	if (!first)
		switch (this->backColor.GetA()) {
			case 255:
				if ((width == this->GetWidth()) && (height == this->GetHeight())) {
					glClearColor(this->backColor.GetR() / 255.0f, this->backColor.GetG() / 255.0f, this->backColor.GetB() / 255.0f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT);
				} else {
					glColor3ub(this->backColor.GetR(), this->backColor.GetG(), this->backColor.GetB());
					GL::FillRectangle(0, 0, this->GetWidth(), this->GetHeight());
				}
				break;
			case 0:
				break;
			default:
				glColor4ub(this->backColor.GetR(), this->backColor.GetG(), this->backColor.GetB(), this->backColor.GetA());
				GL::FillRectangle(0, 0, this->GetWidth(), this->GetHeight());
				break;
		}

	int r = 0;

	Drawing::Rectangle cr = gl->GetClippingResult();
	int isVisible = (cr.GetWidth() > 0) && (cr.GetHeight() > 0) && this->_visible;
	if (isVisible) {
		r++;
		this->OnPaint(NULL);
	}

	if (disableScissor)
		glDisable(GL_SCISSOR_TEST);

	if (isVisible) {
		for (int i = 0; i < this->GetControls()->GetCount(); i++) {
			Control * control = (Control *)this->GetControls()->Get(i);
			if (!control->IsVisible())
				continue;

			GL gl2;
			gl2.SetClippingBase(gl->GetClippingResult());
			r += control->OnPaintPre(x + control->GetLeft(), y + control->GetTop(), width, height, &gl2);
		}
	}

	return r;
}

float Control::Interpolate(float from, int to, float percent) {
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
		if (!control->IsVisible())
			continue;

		if ((focused == control) && (control->GetDrawShadow())) {
			int x1, y1, x2, y2;
			Bitmap * bitmap = Form::GetSelectedBitmap();
			float percent = 0.16f;

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

			int width = control->boundsTo.GetWidth() + x1 + x2;
			int height = control->boundsTo.GetHeight() + y1 + y2;

			lastwidth = this->Interpolate(lastwidth, width, percent);
			lastheight = this->Interpolate(lastheight, height, percent);
			width = Math::Round(lastwidth);
			height = Math::Round(lastheight);

			bitmap->SetSize(width, height);
			int x = control->boundsTo.GetLeft() - x1;
			int y = control->boundsTo.GetTop() - y1;

			lastx1 = this->Interpolate(lastx1, x, percent);
			lasty1 = this->Interpolate(lasty1, y, percent);
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
		if (!control->IsVisible())
			continue;

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
		if (!control->IsVisible())
			continue;

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

	if (!this->_visible)
		return false;

	if (this->GetParent()) {
		Drawing::Rectangle pr = this->GetParent()->GetBounds();
		pr.SetLocation(0, 0);
		Drawing::Rectangle cr = Drawing::Rectangle::Intersect(this->bounds, pr);
		isVisible = (cr.GetWidth() > 0) && (cr.GetHeight() > 0);
	}

	return isVisible;
}

void Control::OnTickPre() {
	float percent = 0.16f;
	_lastWidth = this->Interpolate(_lastWidth, boundsTo.GetWidth(), percent);
	_lastHeight = this->Interpolate(_lastHeight, boundsTo.GetHeight(), percent);
	int w = Math::Round(_lastWidth);
	int h = Math::Round(_lastHeight);
	_lastX = this->Interpolate(_lastX, boundsTo.GetLeft(), percent);
	_lastY = this->Interpolate(_lastY, boundsTo.GetTop(), percent);
	int x = Math::Round(_lastX);
	int y = Math::Round(_lastY);
	this->bounds.SetSize(w, h);
	this->bounds.SetLocation(x, y);

	if (this->IsVisible()) {
		this->OnTick();

		for (int i = 0; i<this->GetControls()->GetCount(); i++) {
			Control * control = (Control *)this->GetControls()->Get(i);
			if (!control->IsVisible())
				continue;
			control->OnTickPre();
		}
	}
}

awui::Drawing::Font * Control::GetFont() {
	return this->font;
}

void Control::SetFont(const Drawing::Font font) {
	*this->font = font;
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

void Control::OnRemoteKeyPressPre(int which, RemoteButtons::Enum button) {
	bool mustStop = this->OnRemoteKeyPress(which, button);
	if (mustStop)
		return;

	if (!this->focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (this->focused)
		this->focused->OnRemoteKeyPressPre(which, button);
}

void Control::OnRemoteKeyUpPre(int which, RemoteButtons::Enum button) {
	bool mustStop = this->OnRemoteKeyUp(which, button);
	if (mustStop)
		return;

	if (!this->focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (this->focused)
		this->focused->OnRemoteKeyUpPre(which, button);
}

void Control::OnKeyPressPre(Keys::Enum key) {
	bool mustStop = this->OnKeyPress(key);
	if (mustStop)
		return;

	if (!this->focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (this->focused)
		this->focused->OnKeyPressPre(key);
}

void Control::OnKeyUpPre(Keys::Enum key) {
	bool mustStop = this->OnKeyUp(key);
	if (mustStop)
		return;

	if (!this->focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (this->focused)
		this->focused->OnKeyUpPre(key);
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

bool Control::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	return false;
}

bool Control::OnKeyUp(Keys::Enum button) {
	return false;
}

// Arriba y Abajo: de la fila de items mas cercana el item mas cercano
// Izquierda y Derecha: De la actual fila, el item mas cercano
bool Control::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	if ((Form::GetControlSelected() == this) && (!this->_preventChangeControl)) {
		Point thisP1(this->GetAbsoluteLeft(), this->GetAbsoluteTop());
		Point thisP2(this->GetAbsoluteRight(), this->GetAbsoluteBottom());;
		Point thisCenter((thisP1.GetX() + thisP2.GetX()) >> 1, (thisP1.GetY() + thisP2.GetY()) >> 1);

		float distance = 30000;
		float distance2 = 30000;
		Control * selected = NULL;

		ArrayList list;
		this->GetTopParent()->GetControlsSelectables(&list);

		switch (button) {
			case RemoteButtons::Left:
				for (int i = 0; i<list.GetCount(); i++) {
					Control * control = (Control *)list.Get(i);
					if (control == this) continue;

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());;
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) >> 1, (controlP1.GetY() + controlP2.GetY()) >> 1);

					if ((controlCenter.GetX()  < thisCenter.GetX()) &&
						(controlP1.GetTop()    < thisP2.GetBottom()) &&
						(controlP2.GetBottom() > thisP1.GetTop())) {
						float calc = Point::Distance(&controlCenter, &thisCenter);
						if (calc < distance) {
							distance = calc;
							selected = control;
						}
					}
				}
				break;
			case RemoteButtons::Right:
				for (int i = 0; i<list.GetCount(); i++) {
					Control * control = (Control *)list.Get(i);
					if (control == this) continue;

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());;
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) >> 1, (controlP1.GetY() + controlP2.GetY()) >> 1);

					if ((controlCenter.GetX()  > thisCenter.GetX()) &&
						(controlP1.GetTop()    < thisP2.GetBottom()) &&
						(controlP2.GetBottom() > thisP1.GetTop())) {
						float calc = Point::Distance(&controlCenter, &thisCenter);
						if (calc < distance) {
							distance = calc;
							selected = control;
						}
					}
				}
				break;
			case RemoteButtons::Down: {
				Control * selectedAux = NULL;
				// Primero busco el mas cercano en Y por abajo
				for (int i = 0; i<list.GetCount(); i++) {
					Control * control = (Control *)list.Get(i);
					if (control == this) continue;

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());;
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) >> 1, (controlP1.GetY() + controlP2.GetY()) >> 1);

					if (controlP1.GetTop() > thisP1.GetBottom()) {
						float calc = controlP1.GetTop() - thisP1.GetTop();
						if (calc < distance) {
							distance = calc;
							distance2 = 30000;
							selectedAux = control;
						} else {
							if (calc == distance) {
								float calc2 = controlCenter.GetY() - thisCenter.GetY();
								if (calc2 < distance2) {
									distance2 = calc2;
									selectedAux = control;
								}
							}
						}
					}
				}

				if (!selectedAux) break;
				distance = 30000;

				// Ahora busco de ese cercano, uno que este en la misma horizontal y mas cerca del actual
				// Y que ademas este por debajo del actual
				for (int i = 0; i<list.GetCount(); i++) {
					Control * control = (Control *)list.Get(i);
					if (control == this) continue;

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());;
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) >> 1, (controlP1.GetY() + controlP2.GetY()) >> 1);

					if ((controlP1.GetTop()    < selectedAux->GetAbsoluteBottom()) &&
					    (controlP2.GetBottom() > selectedAux->GetAbsoluteTop()) &&
						(controlP1.GetTop()    > thisP1.GetBottom())) {
						float calc = Point::Distance(&controlCenter, &thisCenter);
						if (calc < distance) {
							distance = calc;
							selected = control;
						}
					}
				}
			}
				break;
			case RemoteButtons::Up: {
				Control * selectedAux = NULL;
				// Primero busco el mas cercano en Y por arriba
				for (int i = 0; i<list.GetCount(); i++) {
					Control * control = (Control *)list.Get(i);
					if (control == this) continue;

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());;
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) >> 1, (controlP1.GetY() + controlP2.GetY()) >> 1);

					if (controlP2.GetBottom() < thisP2.GetTop()) {
						float calc = thisP2.GetBottom() - controlP2.GetBottom();
						if (calc < distance) {
							distance = calc;
							distance2 = 30000;
							selectedAux = control;
						} else {
							if (calc == distance) {
								float calc2 = thisCenter.GetY() - controlCenter.GetY();
								if (calc2 < distance2) {
									distance2 = calc2;
									selectedAux = control;
								}
							}
						}
					}
				}

				if (!selectedAux) break;
				distance = 30000;

				// Ahora busco de ese cercano, uno que este en la misma horizontal y mas cerca del actual
				// Y ademas que este por arriba del actual
				for (int i = 0; i<list.GetCount(); i++) {
					Control * control = (Control *)list.Get(i);
					if (control == this) continue;

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());;
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) >> 1, (controlP1.GetY() + controlP2.GetY()) >> 1);

					if ((controlP1.GetTop()    < selectedAux->GetAbsoluteBottom()) &&
						(controlP2.GetBottom() > selectedAux->GetAbsoluteTop()) &&
					 	(controlP2.GetBottom() < thisP2.GetTop())) {
						float calc = Point::Distance(&controlCenter, &thisCenter);
						if (calc < distance) {
							distance = calc;
							selected = control;
						}
					}
				}
			}
				break;
			default:
				break;
		}

		if (selected)
			Form::SetControlSelected(selected);

		switch (button) {
			case RemoteButtons::Up:
			case RemoteButtons::Right:
			case RemoteButtons::Down:
			case RemoteButtons::Left:
				break;
			case RemoteButtons::Play:
//				Console::WriteLine("Play");
				break;
			case RemoteButtons::Ok:
//				Console::WriteLine("Ok");
				break;
			case RemoteButtons::Menu:
//				Console::WriteLine("Menu");
				break;
			default:
				break;
		}
	}

	return false;
}

bool Control::OnKeyPress(Keys::Enum key) {
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

void Control::CheckMouseControl() {
	if (this->GetParent() && this->GetParent()->mouseControl == this)
		this->GetParent()->CleanMouseControl();
}

void Control::CleanMouseControl() {
	if (this->mouseControl != NULL)
		this->mouseControl->CleanMouseControl();

	this->mouseControl = NULL;
}
