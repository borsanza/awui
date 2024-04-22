/**
 * awui/Windows/Forms/Control.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Control.h"

#include <SDL_opengl.h>
#include <assert.h>
#include <awui/Console.h>
#include <awui/Drawing/Font.h>
#include <awui/IO/Directory.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/JoystickAxisMotionEventArgs.h>
#include <awui/Windows/Forms/JoystickButtonEventArgs.h>
#include <awui/Windows/Forms/MouseEventArgs.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

Bitmap *Control::m_selectedBitmap = NULL;
int32_t Control::lastTabIndex = 10000;
int32_t Control::countFocused = 1000;

Control::Control() {
	m_class = Classes::Control;
	m_tabIndex = -1;
	m_focusedTime = -1;
	m_focusable = false;
	m_focused = NULL;

	m_deltaSeconds = 0.0f;
	m_refreshed = 0;
	m_lastRight = 0;
	m_lastBottom = 0;
	m_lastLeft = 0;
	m_lastTop = 0;
	m_drawShadow = true;
	m_bounds = Drawing::Rectangle(0, 0, 100, 100);
	m_boundsTo = m_bounds;
	m_controls = new ArrayList();
	m_mouseEventArgs = new MouseEventArgs();
	m_mouseControl = NULL;
	m_parent = NULL;
	m_needRefresh = 1;
	m_dock = DockStyle::None;
	m_backColor = Color::FromArgb(0, 0, 0, 0);
	m_foreColor = Color::FromArgb(255, 255, 255);
	OnResizePre();
	m_font = new Font("sans-serif", 12);
	m_scissorEnabled = true;
	m_preventChangeControl = false;
	m_visible = true;
}

Control::~Control() {
	delete m_font;
	delete m_mouseEventArgs;

	while (m_controls->GetCount() > 0) {
		Control *control = (Control *) m_controls->Get(0);
		delete control;
		m_controls->RemoveAt(0);
	}

	m_controls->Clear();
	delete m_controls;
}

bool Control::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Control) || Object::IsClass(objectClass);
}

void Control::SetTop(int y) {
	SetLocation(m_bounds.GetX(), y);
}

void Control::SetLeft(int x) {
	SetLocation(x, m_bounds.GetY());
}

void Control::SetLocation(int x, int y) {
	SetBounds(x, y, m_bounds.GetWidth(), m_bounds.GetHeight());
}

void Control::SetWidth(int width) {
	SetSize(width, m_bounds.GetHeight());
}

void Control::SetHeight(int height) {
	SetSize(m_bounds.GetWidth(), height);
}

void Control::SetSize(int width, int height) {
	SetBounds(m_bounds.GetX(), m_bounds.GetY(), width, height);
}

void Control::SetSize(const Size size) {
	SetSize(size.GetWidth(), size.GetHeight());
}

void Control::SetBounds(int x, int y, int width, int height) {
	if (width < m_minimumSize.GetWidth())
		width = m_minimumSize.GetWidth();

	if (height < m_minimumSize.GetHeight())
		height = m_minimumSize.GetHeight();

	if ((m_bounds.GetX() == x) && (m_bounds.GetY() == y) && (m_bounds.GetWidth() == width) && (m_bounds.GetHeight() == height))
		return;

	m_bounds = Drawing::Rectangle(x, y, width, height);
	m_boundsTo = m_bounds;
	m_lastRight = m_bounds.GetRight();
	m_lastBottom = m_bounds.GetBottom();
	m_lastLeft = m_bounds.GetLeft();
	m_lastTop = m_bounds.GetTop();

	Refresh();
	OnResizePre();
}

int Control::GetTop() const {
	return m_bounds.GetTop();
}

int Control::GetAbsoluteTop() const {
	Control *parent = GetParent();
	int pos = 0;
	if (parent)
		pos = parent->GetAbsoluteTop();

	return GetTop() + pos;
}

int Control::GetAbsoluteBottom() const {
	return GetAbsoluteTop() + GetHeight() - 1;
}

int Control::GetLeft() const {
	return m_bounds.GetLeft();
}

int Control::GetAbsoluteLeft() const {
	Control *parent = GetParent();
	int pos = 0;
	if (parent)
		pos = parent->GetAbsoluteLeft();

	return GetLeft() + pos;
}

int Control::GetAbsoluteRight() const {
	return GetAbsoluteLeft() + GetWidth() - 1;
}

int Control::GetRight() const {
	return m_bounds.GetRight();
}

int Control::GetBottom() const {
	return m_bounds.GetBottom();
}

const Point Control::GetLocation() const {
	return m_bounds.GetLocation();
}

int Control::GetWidth() const {
	return m_bounds.GetWidth();
}

int Control::GetHeight() const {
	return m_bounds.GetHeight();
}

const Size Control::GetSize() const {
	return m_bounds.GetSize();
}

const awui::Drawing::Rectangle Control::GetBounds() const {
	return m_bounds;
}

void Control::MoveToEnd(Control *item) {
	RemoveWidget(item);
	AddWidget(item);
}

void Control::OnResizePre() {
	OnResize();
	Layout();
}

void Control::SetBackColor(const awui::Color color) {
	m_backColor = color;
}

awui::Color Control::GetBackColor() {
	return m_backColor;
}

void Control::SetForeColor(const awui::Color color) {
	m_foreColor = color;
}

awui::Color Control::GetForeColor() {
	return m_foreColor;
}

void Control::SetDock(DockStyle dock) {
	if (m_dock != dock) {
		m_dock = dock;
		Layout();
	}
}

DockStyle Control::GetDock() const {
	return m_dock;
}

const Size Control::GetMinimumSize() const {
	return m_minimumSize;
}

void Control::SetMinimumSize(Size size) {
	m_minimumSize = size;
	SetSize(GetSize());
}

Control *Control::GetParent() const {
	return m_parent;
}

void Control::SetParent(Control *parent) {
	m_parent = parent;
}

void Control::Layout() {
	int x1 = 0;
	int y1 = 0;
	int x2 = GetWidth() - 1;
	int y2 = GetHeight() - 1;
	int margin = 0;

	for (int i = 0; i < GetCount(); i++) {
		Control *control = Get(i);
		if (!control->IsVisible(false)) {
			continue;
		}

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
	if (GetParent())
		GetParent()->Refresh();

	m_needRefresh = 1;
}

int Control::OnPaintPre(int x, int y, int width, int height, GL *gl, bool first) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-x, width - x, height - y, -y, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Drawing::Rectangle rect2;
	rect2.SetX(x);
	rect2.SetY(height - y - GetHeight());
	rect2.SetWidth(GetWidth());
	rect2.SetHeight(GetHeight());
	gl->SetClipping(rect2);

	bool disableScissor = false;
	if (m_scissorEnabled) {
		disableScissor = true;
		glEnable(GL_SCISSOR_TEST);
		gl->SetClipping();
	}

	if (!first)
		switch (m_backColor.GetA()) {
			case 255:
				if ((width == GetWidth()) && (height == GetHeight())) {
					glClearColor(m_backColor.GetR() / 255.0f, m_backColor.GetG() / 255.0f, m_backColor.GetB() / 255.0f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT);
				} else {
					glColor3ub(m_backColor.GetR(), m_backColor.GetG(), m_backColor.GetB());
					GL::FillRectangle(0, 0, GetWidth(), GetHeight());
				}
				break;
			case 0:
				break;
			default:
				glColor4ub(m_backColor.GetR(), m_backColor.GetG(), m_backColor.GetB(), m_backColor.GetA());
				GL::FillRectangle(0, 0, GetWidth(), GetHeight());
				break;
		}

	int r = 0;

	Drawing::Rectangle cr = gl->GetClippingResult();
	int isVisible = (cr.GetWidth() > 0) && (cr.GetHeight() > 0) && m_visible;
	if (isVisible) {
		r++;
		OnPaint(NULL);
	}

	if (disableScissor)
		glDisable(GL_SCISSOR_TEST);

	if (isVisible) {
		for (int i = 0; i < GetCount(); i++) {
			Control *control = Get(i);
			if (!control->IsVisible())
				continue;

			GL gl2;
			gl2.SetClippingBase(gl->GetClippingResult());
			r += control->OnPaintPre(x + control->GetLeft(), y + control->GetTop(), width, height, &gl2);
		}
	}

	return r;
}

// Lo usamos para dibujar el skin
void Control::OnPaint(OpenGL::GL *gl) {
	static float lastx1, lasty1, lastright, lastbottom;
	static Control *lastParent = NULL;
	Control *focused = nullptr;
	Form *form = GetForm();
	if (form) {
		focused = form->GetChildFocused();
	}

	for (int i = 0; i < GetCount(); i++) {
		Control *control = Get(i);
		if (!control->IsVisible()) {
			continue;
		}

		if ((focused == control) && (control->GetDrawShadow())) {
			int x1, y1, x2, y2;
			Bitmap *bitmap = Control::GetSelectedBitmap();
			float percent = m_deltaSeconds * 10.0f;

			if (lastParent != this) {
				lastParent = this;
				percent = 1.0f;
			}

			bitmap->GetFixedMargins(&x1, &y1, &x2, &y2);
			float right = control->m_bounds.GetRight() + x2;
			float bottom = control->m_bounds.GetBottom() + y2;

			lastright = Math::Interpolate(lastright, right, percent);
			lastbottom = Math::Interpolate(lastbottom, bottom, percent);

			lastx1 = Math::Interpolate(lastx1, control->m_bounds.GetLeft() - x1, percent);
			lasty1 = Math::Interpolate(lasty1, control->m_bounds.GetTop() - y1, percent);

			bitmap->SetSize(Math::Round(lastright - lastx1 + 1.0f), Math::Round(lastbottom - lasty1 + 1.0f));

			glTranslatef(Math::Round(lastx1), Math::Round(lasty1), 0);
			bitmap->OnPaint(NULL);
			glTranslatef(-Math::Round(lastx1), -Math::Round(lasty1), 0);
		}
	}
}

void Control::OnMouseDownPre(int x, int y, MouseButtons::Enum button, int buttons) {
	m_mouseEventArgs->SetLocation(x, y);

	for (int i = GetCount() - 1; i >= 0; i--) {
		Control *control = Get(i);
		if (!control->IsVisible())
			continue;

		if (m_mouseControl != NULL) {
			if (m_mouseControl == control) {
				control->OnMouseDownPre(x - control->GetLeft(), y - control->GetTop(), button, buttons);
				return;
			}
		} else {
			if ((control->GetLeft() <= x) && (x <= control->GetRight()) && (control->GetTop() <= y) && (y <= control->GetBottom())) {
				m_mouseControl = control;
				ChangeControlOnMouseOver(control);
				control->OnMouseDownPre(x - control->GetLeft(), y - control->GetTop(), button, buttons);
				return;
			}
		}
	}

	m_mouseEventArgs->SetButton(button);
	OnMouseDown(m_mouseEventArgs);

	//	std::cout << "Down: " << mouseEventArgs->GetX() << "x" << mouseEventArgs->GetY() << "   " << mouseEventArgs->GetButton() << "   " << GetName() << std::endl;
}

void Control::OnMouseMovePre(int x, int y, int buttons) {
	m_mouseEventArgs->SetLocation(x, y);

	for (int i = GetCount() - 1; i >= 0; i--) {
		Control *control = Get(i);
		if (!control->IsVisible())
			continue;

		int find = 0;

		if (m_mouseControl != NULL) {
			if (m_mouseControl == control)
				find = 1;
		} else {
			if ((control->GetLeft() <= x) && (x <= control->GetRight()) && (control->GetTop() <= y) && (y <= control->GetBottom()))
				find = 1;
		}

		if (find) {
			ChangeControlOnMouseOver(control);

			control->OnMouseMovePre(x - control->GetLeft(), y - control->GetTop(), buttons);
			return;
		}
	}

	m_mouseEventArgs->SetButton(buttons);
	OnMouseMove(m_mouseEventArgs);
	Refresh();

	//	std::cout << "Move: " << mouseEventArgs->GetX() << "x" << mouseEventArgs->GetY() << "   " << mouseEventArgs->GetButton() << "   " << GetName() << std::endl;
}

void Control::ChangeControlOnMouseOver(Control *control) {
	if (GetParent()) {
		GetParent()->ChangeControlOnMouseOver(control);
		return;
	}

	if (IsClass(Classes::Form)) {
		if (((Form *) this)->m_mouseControlOver != control) {
			if (((Form *) this)->m_mouseControlOver != NULL)
				((Form *) this)->m_mouseControlOver->OnMouseLeave();

			((Form *) this)->m_mouseControlOver = control;
			control->OnMouseEnter();
		}
	}
}

void Control::OnMouseUpPre(MouseButtons::Enum button, int buttons) {
	int x = m_mouseEventArgs->GetX();
	int y = m_mouseEventArgs->GetY();

	for (int i = GetCount() - 1; i >= 0; i--) {
		Control *control = Get(i);
		if (!control->IsVisible())
			continue;

		if (m_mouseControl != NULL) {
			if (m_mouseControl == control) {
				m_mouseControl = NULL;
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

	m_mouseEventArgs->SetButton(button);
	OnMouseUp(m_mouseEventArgs);

	//	std::cout << "Up: " << mouseEventArgs->GetX() << "x" << mouseEventArgs->GetY() << "   " << mouseEventArgs->GetButton() << "   " << GetName() << std::endl;
}

void Control::OnMouseLeave() {
	//	std::cout << "OnMouseLeave: " << GetName() << std::endl;
}

void Control::OnMouseEnter() {
	//	std::cout << "OnMouseEnter: " << GetName() << std::endl;
}

void Control::SetName(const String str) {
	m_name = str;
}

const awui::String Control::GetName() {
	return m_name;
}

bool Control::IsVisible(bool checkInside) const {
	bool isVisible = true;

	if (!m_visible) {
		return false;
	}

	if (checkInside && GetParent()) {
		Drawing::Rectangle pr = GetParent()->GetBounds();
		pr.SetLocation(0, 0);
		Drawing::Rectangle cr = Drawing::Rectangle::Intersect(m_bounds, pr);
		isVisible = GetParent()->IsVisible() && ((cr.GetWidth() > 0) && (cr.GetHeight() > 0));
	}

	return isVisible;
}

#include <inttypes.h>

void Control::OnTickPre(float deltaSeconds) {
	m_deltaSeconds = deltaSeconds;
	float percent = 10.0f * deltaSeconds;
	// awui::Console::WriteLine("%lld %.3f", ellapsed, percent);

	m_lastRight = Math::Interpolate(m_lastRight, m_boundsTo.GetRight(), percent);
	m_lastBottom = Math::Interpolate(m_lastBottom, m_boundsTo.GetBottom(), percent);
	m_lastLeft = Math::Interpolate(m_lastLeft, m_boundsTo.GetLeft(), percent);
	m_lastTop = Math::Interpolate(m_lastTop, m_boundsTo.GetTop(), percent);
	m_bounds.SetSize(m_lastRight - m_lastLeft + 1.0f, m_lastBottom - m_lastTop + 1.0f);
	m_bounds.SetLocation(m_lastLeft, m_lastTop);

	if (IsVisible()) {
		OnTick(deltaSeconds);

		for (int i = 0; i < GetCount(); i++) {
			Control *control = Get(i);
			if (!control->IsVisible()) {
				continue;
			}
			control->OnTickPre(deltaSeconds);
		}
	}
}

awui::Drawing::Font *Control::GetFont() {
	return m_font;
}

void Control::SetFont(const Drawing::Font font) {
	*m_font = font;
}

void Control::SetScissorEnabled(bool mode) {
	m_scissorEnabled = mode;
}

bool Control::GetScissorEnabled() {
	return m_scissorEnabled;
}

void Control::OnRemoteKeyPressPre(int which, RemoteButtons::Enum button) {
	bool mustStop = IsVisible() && OnRemoteKeyPress(which, button);
	if (mustStop) {
		return;
	}

	if (m_focused && m_focused->IsVisible()) {
		m_focused->OnRemoteKeyPressPre(which, button);
	}
}

void Control::OnRemoteKeyUpPre(int which, RemoteButtons::Enum button) {
	bool mustStop = IsVisible() && OnRemoteKeyUp(which, button);
	if (mustStop) {
		return;
	}

	if (m_focused && m_focused->IsVisible()) {
		m_focused->OnRemoteKeyUpPre(which, button);
	}
}

void Control::OnJoystickButtonDownPre(int which, int button, uint32_t buttons, uint32_t prevButtons) {
	JoystickButtonEventArgs joy(which, button, buttons, prevButtons);

	bool mustStop = OnJoystickButtonDown(&joy);
	if (mustStop) {
		return;
	}

	if (m_focused) {
		m_focused->OnJoystickButtonDownPre(which, button, buttons, prevButtons);
	}
}

void Control::OnJoystickButtonUpPre(int which, int button, uint32_t buttons, uint32_t prevButtons) {
	JoystickButtonEventArgs joy(which, button, buttons, prevButtons);

	bool mustStop = OnJoystickButtonUp(&joy);
	if (mustStop) {
		return;
	}

	if (m_focused) {
		m_focused->OnJoystickButtonUpPre(which, button, buttons, prevButtons);
	}
}
void Control::OnJoystickAxisMotionPre(int which, int16_t axisX, int16_t axisY) {
	JoystickAxisMotionEventArgs joy(which, axisX, axisY);

	bool mustStop = OnJoystickAxisMotion(&joy);
	if (mustStop) {
		return;
	}

	if (m_focused) {
		m_focused->OnJoystickAxisMotionPre(which, axisX, axisY);
	}
}

void Control::OnKeyPressPre(Keys::Enum key) {
	bool mustStop = IsVisible() && OnKeyPress(key);
	if (mustStop) {
		return;
	}

	if (m_focused && m_focused->IsVisible()) {
		m_focused->OnKeyPressPre(key);
	}
}

void Control::OnKeyUpPre(Keys::Enum key) {
	bool mustStop = IsVisible() && OnKeyUp(key);
	if (mustStop) {
		return;
	}

	if (m_focused && m_focused->IsVisible()) {
		m_focused->OnKeyUpPre(key);
	}
}

Control *Control::GetRoot() {
	if (m_parent) {
		return m_parent->GetRoot();
	}

	return this;
}

Form *Control::GetForm() {
	const Control *root = GetRoot();

	if (!root->IsClass(Classes::Form)) {
		return nullptr;
	};

	return (Form *) root;
}

bool Control::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	return false;
}

bool Control::OnJoystickButtonDown(JoystickButtonEventArgs *e) {
	return false;
}

bool Control::OnJoystickButtonUp(JoystickButtonEventArgs *e) {
	return false;
}

bool Control::OnJoystickAxisMotion(JoystickAxisMotionEventArgs *e) {
	return false;
}

bool Control::OnKeyUp(Keys::Enum button) {
	return false;
}

// Arriba y Abajo: de la fila de items mas cercana el item mas cercano
// Izquierda y Derecha: De la actual fila, el item mas cercano
bool Control::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	Form *form = GetForm();
	if (!form) {
		return false;
	}

	if ((form->GetChildFocused() == this) && (!m_preventChangeControl)) {
		Point thisP1(GetAbsoluteLeft(), GetAbsoluteTop());
		Point thisP2(GetAbsoluteRight(), GetAbsoluteBottom());
		Point thisCenter((thisP1.GetX() + thisP2.GetX()) / 2.0, (thisP1.GetY() + thisP2.GetY()) / 2.0);

		float distance = 30000;
		float distance2 = 30000;
		Control *selected = NULL;

		ArrayList list;
		GetRoot()->GetControlsSelectables(&list);

		switch (button) {
			case RemoteButtons::Left:
				for (int i = 0; i < list.GetCount(); i++) {
					Control *control = (Control *) list.Get(i);
					if (!control->IsVisible() || (control == this)) {
						continue;
					}

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) / 2.0, (controlP1.GetY() + controlP2.GetY()) / 2.0);

					if ((controlCenter.GetX() < thisCenter.GetX()) && (controlP1.GetTop() < thisP2.GetBottom()) && (controlP2.GetBottom() > thisP1.GetTop())) {
						float calc = Point::Distance(&controlCenter, &thisCenter);
						if (calc < distance) {
							distance = calc;
							selected = control;
						}
					}
				}
				break;
			case RemoteButtons::Right:
				for (int i = 0; i < list.GetCount(); i++) {
					Control *control = (Control *) list.Get(i);
					if (!control->IsVisible() || (control == this)) {
						continue;
					}

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) / 2.0, (controlP1.GetY() + controlP2.GetY()) / 2.0);

					if ((controlCenter.GetX() > thisCenter.GetX()) && (controlP1.GetTop() < thisP2.GetBottom()) && (controlP2.GetBottom() > thisP1.GetTop())) {
						float calc = Point::Distance(&controlCenter, &thisCenter);
						if (calc < distance) {
							distance = calc;
							selected = control;
						}
					}
				}
				break;
			case RemoteButtons::Down: {
				Control *selectedAux = NULL;
				// Primero busco el mas cercano en Y por abajo
				for (int i = 0; i < list.GetCount(); i++) {
					Control *control = (Control *) list.Get(i);
					if (!control->IsVisible() || (control == this)) {
						continue;
					}

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) / 2.0, (controlP1.GetY() + controlP2.GetY()) / 2.0);

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

				if (!selectedAux) {
					break;
				}

				distance = 30000;

				// Ahora busco de ese cercano, uno que este en la misma horizontal y mas cerca del actual
				// Y que ademas este por debajo del actual
				for (int i = 0; i < list.GetCount(); i++) {
					Control *control = (Control *) list.Get(i);
					if (!control->IsVisible() || (control == this)) {
						continue;
					}

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) / 2.0, (controlP1.GetY() + controlP2.GetY()) / 2.0);

					if ((controlP1.GetTop() < selectedAux->GetAbsoluteBottom()) && (controlP2.GetBottom() > selectedAux->GetAbsoluteTop()) && (controlP1.GetTop() > thisP1.GetBottom())) {
						float calc = Point::Distance(&controlCenter, &thisCenter);
						if (calc < distance) {
							distance = calc;
							selected = control;
						}
					}
				}
				break;
			}
			case RemoteButtons::Up: {
				Control *selectedAux = NULL;
				// Primero busco el mas cercano en Y por arriba
				for (int i = 0; i < list.GetCount(); i++) {
					Control *control = (Control *) list.Get(i);
					if (!control->IsVisible() || (control == this)) {
						continue;
					}

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) / 2.0, (controlP1.GetY() + controlP2.GetY()) / 2.0);

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

				if (!selectedAux) {
					break;
				}

				distance = 30000;

				// Ahora busco de ese cercano, uno que este en la misma horizontal y mas cerca del actual
				// Y ademas que este por arriba del actual
				for (int i = 0; i < list.GetCount(); i++) {
					Control *control = (Control *) list.Get(i);
					if (!control->IsVisible() || (control == this)) {
						continue;
					}

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) / 2.0, (controlP1.GetY() + controlP2.GetY()) / 2.0);

					if ((controlP1.GetTop() < selectedAux->GetAbsoluteBottom()) && (controlP2.GetBottom() > selectedAux->GetAbsoluteTop()) && (controlP2.GetBottom() < thisP2.GetTop())) {
						float calc = Point::Distance(&controlCenter, &thisCenter);
						if (calc < distance) {
							distance = calc;
							selected = control;
						}
					}
				}
				break;
			}
			default:
				break;
		}

		if (selected) {
			Form *form = selected->GetForm();
			if (form) {
				Control *focused = form->GetChildFocused();
				if (focused) {
					focused->m_focusedTime = -1;
				}
			}
			selected->SetFocus();
		}
	}

	return false;
}

bool Control::OnKeyPress(Keys::Enum key) {
	return false;
}

void Control::GetControlsSelectables(ArrayList *list) {
	for (int i = 0; i < GetCount(); i++) {
		Control *control = Get(i);
		control->GetControlsSelectables(list);
	}

	if (IsFocusable())
		list->Add(this);
}

void Control::CheckMouseControl() {
	if (GetParent() && GetParent()->m_mouseControl == this)
		GetParent()->CleanMouseControl();
}

void Control::CleanMouseControl() {
	if (m_mouseControl != NULL)
		m_mouseControl->CleanMouseControl();

	m_mouseControl = NULL;
}

Bitmap *Control::GetSelectedBitmap() {
	if (!Control::m_selectedBitmap) {
		String file = IO::Directory::GetWorkingDirectory();
		Bitmap *bitmap = new Bitmap(file + "/images/button.png");
		bitmap->SetDock(DockStyle::None);
		bitmap->SetBackColor(Color::FromArgb(0, 0, 0, 0));
		bitmap->SetFixedMargins(28, 25, 28, 24);
		bitmap->SetLocation(0, 0);
		bitmap->SetSize(Drawing::Size(97, 97));
		Control::m_selectedBitmap = bitmap;
	}

	return Control::m_selectedBitmap;
}

awui::String Control::ToString() const {
	return "awui::Windows::Forms::Control";
}

/******************************************************************************/
/*********************** Actions that affect the focus ************************/
/******************************************************************************/

/**
 * Indica si el control puede recibir el foco.
 *
 * @return True si el control es enfocable, False de lo contrario.
 */
bool Control::IsFocusable() {
	return m_focusable;
}

/**
 * Establece la capacidad del control para recibir o no el foco.
 *
 * @param focusable Especifica si el control debe ser enfocable (true) o no (false).
 */
void Control::SetFocusable(bool focusable) {
	m_focusable = focusable;
}

/**
 * Establece el enfoque en este control y actualiza la jerarquía de enfoque si es necesario.
 *
 * @param forced Si el enfoque debe ser forzado.
 * @param time Prioridad o momento del enfoque.
 */
void Control::SetFocusImpl(bool forced, int32_t time) {
	if (forced || time > m_focusedTime) {
		m_focusedTime = time;
	}

	if (m_parent && (forced || m_parent->m_focused == nullptr || m_parent->m_focused->m_focusedTime < time)) {
		m_parent->m_focused = this;
		m_parent->SetFocusImpl(forced, time);
	}
}

/**
 * Establece o actualiza el enfoque en este control. Si se fuerza el enfoque, se incrementa
 * el contador de enfoque; de lo contrario, se utiliza el tiempo de enfoque actual.
 *
 * @param forced Indica si el enfoque debe ser forzado.
 */
void Control::SetFocus(bool forced) {
	SetFocusImpl(forced, forced ? countFocused : m_focusedTime);

	if (forced) {
		countFocused++;
	}
}

/**
 * Busca recursivamente el control enfocado más profundo, iniciando desde este control.
 *
 * @param focused Control enfocado inicial; suele ser null en la primera llamada.
 * @return Control enfocado más profundo.
 */
Control *Control::GetChildFocusedImp(Control *focused) {
	if (IsFocusable())
		focused = this;

	if (m_focused) {
		return m_focused->GetChildFocusedImp(focused);
	}

	return focused;
}

/**
 * Inicia la búsqueda del control enfocado dentro de la jerarquía de este control.
 *
 * @return El control enfocado más profundo o null si ninguno está enfocado.
 */
Control *Control::GetChildFocused() {
	return GetChildFocusedImp(nullptr);
}

/**
 * Verifica si el control está enfocado, considerando la jerarquía de contenedores hasta un formulario.
 *
 * @return True si está enfocado, False si no.
 */
bool Control::IsFocused() const {
	Control *parent = GetParent();
	if (parent) {
		return (parent->m_focused == this) && parent->IsFocused();
	}

	return IsClass(Classes::Form);
}

/**
 * Agrega un nuevo control al contenedor si no está ya presente y no tiene otro contenedor.
 * Asigna un índice de tabulación si aún no tiene uno, lo agrega a la colección de controles,
 * establece este contenedor como su padre, y luego ajusta el enfoque y la disposición del contenedor.
 *
 * @param control El control a agregar.
 */
void Control::AddWidget(Control *control) {
	assert(control && !m_controls->Contains(control) && control->GetParent() == nullptr);
	if (!control || m_controls->Contains(control) || control->GetParent() != nullptr) {
		return;
	}

	if (control->m_tabIndex == -1) {
		control->m_tabIndex = lastTabIndex++;
	}

	m_controls->Add(control);
	control->SetParent(this);

	FixFocusImpl();

	Layout();
}

/**
 * Elimina un control específico del contenedor, verificando primero que el control exista en este contenedor
 * y que realmente pertenezca a él como hijo. Después de la eliminación, el control se desvincula de este contenedor
 * y se realiza una revisión para ajustar el enfoque dentro del contenedor, si es necesario.
 *
 * @param control El control a eliminar del contenedor.
 */
void Control::RemoveWidget(Control *control) {
	assert(control && m_controls->Contains(control) && control->GetParent() == this);
	if (!control || !m_controls->Contains(control) || control->GetParent() != this) {
		return;
	}

	m_controls->Remove(control);
	control->SetParent(nullptr);

	FixFocusImpl();
}

/**
 * Reemplaza un control antiguo por uno nuevo en el contenedor. Asegura que ambos controles sean válidos,
 * diferentes, y que el nuevo control no tenga ya un contenedor. Actualiza la jerarquía de controles y el enfoque.
 *
 * @param oldControl Control a reemplazar.
 * @param newControl Nuevo control a insertar.
 */
void Control::ReplaceWidget(Control *oldControl, Control *newControl) {
	assert((oldControl != nullptr) && (newControl != nullptr) && (oldControl != newControl) && (m_controls->Contains(oldControl)) && newControl->GetParent() == nullptr);
	if (oldControl == nullptr || newControl == nullptr || oldControl == newControl || !m_controls->Contains(oldControl) || newControl->GetParent() != nullptr) {
		return;
	}

	oldControl->CheckMouseControl();
	oldControl->SetParent(nullptr);
	m_controls->Replace(oldControl, newControl);
	newControl->SetParent(this);

	FixFocusImpl();
}

/**
 * Establece la visibilidad del control y actualiza el enfoque en el padre si es necesario.
 * Solo actúa si la nueva visibilidad difiere de la actual.
 *
 * @param isVisible True para hacer el control visible, false para ocultarlo.
 */
void Control::SetVisible(bool isVisible) {
	if (m_visible != isVisible) {
		m_visible = isVisible;

		if (m_parent) {
			m_parent->FixFocusImpl();
		}
	}
}

/**
 * Busca y devuelve el próximo control elegible para recibir el enfoque dentro del contenedor de controles.
 * El criterio para determinar el "próximo" control se basa en la visibilidad del control y su tiempo de enfoque.
 * Solo los controles visibles son considerados elegibles, y entre ellos, se prefiere aquel con el mayor tiempo de enfoque.
 *
 * @return Control* Puntero al control que ha sido identificado como el próximo elegible para recibir el enfoque.
 *         Retorna nullptr si no se encuentra ningún control elegible.
 */
Control *Control::FindNextFocusableControl() {
	Control *nextFocusable = nullptr;
	int32_t highestFocusTime = -1;
	for (auto aux : *m_controls) {
		Control *control = (Control *) aux;
		if (control->IsVisible() && (nextFocusable == nullptr || control->m_focusedTime > highestFocusTime)) {
			nextFocusable = control;
			highestFocusTime = control->m_focusedTime;
		}
	}

	return nextFocusable;
}

/**
 * Actualiza el control enfocado al próximo elegible y notifica al contenedor padre para ajustar su enfoque.
 */
void Control::FixFocusImpl() {
	Control *nextFocus = FindNextFocusableControl();

	if (nextFocus != m_focused) {
		m_focused = nextFocus;
		m_focusedTime = (m_focused != nullptr) ? m_focused->m_focusedTime : -1;

		if (m_parent) {
			m_parent->FixFocusImpl();
		}
	}
}
