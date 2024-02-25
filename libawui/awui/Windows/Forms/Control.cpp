/**
 * awui/Windows/Forms/Control.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Control.h"

#include <awui/Console.h>
#include <awui/Drawing/Font.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/JoystickButtonEventArgs.h>
#include <awui/Windows/Forms/JoystickAxisMotionEventArgs.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

Control::Control() {
	m_deltaSeconds = 0.0f;
	m_refreshed = 0;
	m_lastWidth = 0;
	m_lastHeight = 0;
	m_lastX = 0;
	m_lastY = 0;
	m_drawShadow = true;
	m_tabStop = false;
	m_bounds = Drawing::Rectangle(0, 0, 100, 100);
	m_boundsTo = m_bounds;
	m_controls = new ControlCollection(this);
	m_mouseEventArgs = new MouseEventArgs();
	m_mouseControl = NULL;
	m_parent = NULL;
	m_focused = NULL;
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
		Control * control = (Control *) m_controls->Get(0);
		delete control;
		m_controls->RemoveAt(0);
	}

	m_controls->Clear();
	delete m_controls;
}

bool Control::IsClass(Classes objectClass) const {
	if (objectClass == Classes::Control) {
		return true;
	}

	return Object::IsClass(objectClass);
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
	m_lastWidth = width;
	m_lastHeight = height;
	m_lastX = x;
	m_lastY = y;

	Refresh();
	OnResizePre();
}

int Control::GetTop() const {
	return m_bounds.GetTop();
}

int Control::GetAbsoluteTop() const {
	Control * parent = GetParent();
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
	Control * parent = GetParent();
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

ControlCollection* Control::GetControls() const {
	return m_controls;
}

void Control::OnResizePre() {
	OnResize();
	Layout();
}

void Control::SetBackColor(const awui::Drawing::Color color) {
	m_backColor = color;
}

awui::Drawing::Color Control::GetBackColor() {
	return m_backColor;
}

void Control::SetForeColor(const awui::Drawing::Color color) {
	m_foreColor = color;
}

awui::Drawing::Color Control::GetForeColor() {
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

Control * Control::GetParent() const {
	return m_parent;
}

void Control::SetParent(Control * parent) {
	m_parent = parent;
}

void Control::Layout() {
	int x1 = 0;
	int y1 = 0;
	int x2 = GetWidth() - 1;
	int y2 = GetHeight() - 1;
	int margin = 0;

	for (int i = 0; i < GetControls()->GetCount(); i++) {
		Control * control = (Control *)GetControls()->Get(i);
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
	if (GetParent())
		GetParent()->Refresh();

	m_needRefresh = 1;
}

int Control::OnPaintPre(int x, int y, int width, int height, GL * gl, bool first) {
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
		for (int i = 0; i < GetControls()->GetCount(); i++) {
			Control * control = (Control *)GetControls()->Get(i);
			if (!control->IsVisible())
				continue;

			GL gl2;
			gl2.SetClippingBase(gl->GetClippingResult());
			r += control->OnPaintPre(x + control->GetLeft(), y + control->GetTop(), width, height, &gl2);
		}
	}

	return r;
}

float Control::Interpolate(float from, float to, float percent) {
	if (awui::Math::Round(from) == awui::Math::Round(to))
		return to;

	return from + ((to - from) * percent);
}

// Lo usamos para dibujar el skin
void Control::OnPaint(OpenGL::GL * gl) {
	static float lastx1, lasty1, lastwidth, lastheight;
	static Control * lastParent = NULL;
	Control * focused = Form::GetControlSelected();

	for (int i = 0; i < GetControls()->GetCount(); i++) {
		Control * control = (Control *)GetControls()->Get(i);
		if (!control->IsVisible())
			continue;

		if ((focused == control) && (control->GetDrawShadow())) {
			int x1, y1, x2, y2;
			Bitmap * bitmap = Form::GetSelectedBitmap();
			float percent = m_deltaSeconds * 10.0f;

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

			int width = control->m_boundsTo.GetWidth() + x1 + x2;
			int height = control->m_boundsTo.GetHeight() + y1 + y2;

			lastwidth = Interpolate(lastwidth, width, percent);
			lastheight = Interpolate(lastheight, height, percent);
			width = Math::Round(lastwidth);
			height = Math::Round(lastheight);

			bitmap->SetSize(width, height);
			int x = control->m_boundsTo.GetLeft() - x1;
			int y = control->m_boundsTo.GetTop() - y1;

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
	m_mouseEventArgs->SetLocation(x, y);

	for (int i = GetControls()->GetCount() - 1; i >= 0; i--) {
		Control * control = (Control *)GetControls()->Get(i);
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

	for (int i = GetControls()->GetCount() - 1; i >= 0; i--) {
		Control * control = (Control *)GetControls()->Get(i);
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

void Control::ChangeControlOnMouseOver(Control * control) {
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

	for (int i = GetControls()->GetCount() - 1; i >= 0; i--) {
		Control * control = (Control *)GetControls()->Get(i);
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

bool Control::IsVisible() const {
	bool isVisible = true;

	if (!m_visible) {
		return false;
	}

	if (GetParent()) {
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
	double percent = 10.0f * deltaSeconds;
	//awui::Console::WriteLine("%lld %.3f", ellapsed, percent);

	m_lastWidth = Interpolate(m_lastWidth, m_boundsTo.GetWidth(), percent);
	m_lastHeight = Interpolate(m_lastHeight, m_boundsTo.GetHeight(), percent);
	int w = m_lastWidth;  //Math::Round(m_lastWidth);
	int h = m_lastHeight; //Math::Round(m_lastHeight);
	m_lastX = Interpolate(m_lastX, m_boundsTo.GetLeft(), percent);
	m_lastY = Interpolate(m_lastY, m_boundsTo.GetTop(), percent);
	int x = m_lastX; // Math::Round(m_lastX);
	int y = m_lastY; // Math::Round(m_lastY);
	m_bounds.SetSize(w, h);
	m_bounds.SetLocation(x, y);

	if (IsVisible()) {
		OnTick(deltaSeconds);

		for (int i = 0; i<GetControls()->GetCount(); i++) {
			Control * control = (Control *)GetControls()->Get(i);
			if (!control->IsVisible())
				continue;
			control->OnTickPre(deltaSeconds);
		}
	}
}

awui::Drawing::Font * Control::GetFont() {
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

bool Control::GetTabStop() {
	return m_tabStop;
}

void Control::SetTabStop(bool tabStop) {
	m_tabStop = tabStop;
}

void Control::OnRemoteKeyPressPre(int which, RemoteButtons::Enum button) {
	bool mustStop = IsVisible() && OnRemoteKeyPress(which, button);
	if (mustStop)
		return;

	if (!m_focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (m_focused && m_focused->IsVisible())
		m_focused->OnRemoteKeyPressPre(which, button);
}

void Control::OnRemoteKeyUpPre(int which, RemoteButtons::Enum button) {
	bool mustStop = IsVisible() && OnRemoteKeyUp(which, button);
	if (mustStop)
		return;

	if (!m_focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (m_focused && m_focused->IsVisible())
		m_focused->OnRemoteKeyUpPre(which, button);
}

void Control::OnJoystickButtonDownPre(int which, int button, uint32_t buttons, uint32_t prevButtons) {
	JoystickButtonEventArgs joy(which, button, buttons, prevButtons);
	bool mustStop = OnJoystickButtonDown(&joy);
	if (mustStop)
		return;

	if (!m_focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (m_focused)
		m_focused->OnJoystickButtonDownPre(which, button, buttons, prevButtons);
}

void Control::OnJoystickButtonUpPre(int which, int button, uint32_t buttons, uint32_t prevButtons) {
	JoystickButtonEventArgs joy(which, button, buttons, prevButtons);
	bool mustStop = OnJoystickButtonUp(&joy);
	if (mustStop)
		return;

	if (!m_focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (m_focused)
		m_focused->OnJoystickButtonUpPre(which, button, buttons, prevButtons);
}
void Control::OnJoystickAxisMotionPre(int which, int16_t axisX, int16_t axisY) {
	JoystickAxisMotionEventArgs joy(which, axisX, axisY);
	bool mustStop = OnJoystickAxisMotion(&joy);
	if (mustStop)
		return;

	if (!m_focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (m_focused)
		m_focused->OnJoystickAxisMotionPre(which, axisX, axisY);
}



void Control::OnKeyPressPre(Keys::Enum key) {
	bool mustStop = IsVisible() && OnKeyPress(key);
	if (mustStop)
		return;

	if (!m_focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (m_focused && m_focused->IsVisible())
		m_focused->OnKeyPressPre(key);
}

void Control::OnKeyUpPre(Keys::Enum key) {
	bool mustStop = IsVisible() && OnKeyUp(key);
	if (mustStop)
		return;

	if (!m_focused)
		Form::SetControlSelected(Form::GetControlSelected());

	if (m_focused && m_focused->IsVisible())
		m_focused->OnKeyUpPre(key);
}

void Control::SetFocus(bool selectControl) {
	Control * parent = GetParent();
	if (parent) {
		if (selectControl)
			Form::SetControlSelected(this);
		parent->m_focused = this;
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

bool Control::OnJoystickButtonDown(JoystickButtonEventArgs* e) {
	return false;
}

bool Control::OnJoystickButtonUp(JoystickButtonEventArgs* e) {
	return false;
}

bool Control::OnJoystickAxisMotion(JoystickAxisMotionEventArgs* e) {
	return false;
}

bool Control::OnKeyUp(Keys::Enum button) {
	return false;
}

// Arriba y Abajo: de la fila de items mas cercana el item mas cercano
// Izquierda y Derecha: De la actual fila, el item mas cercano
bool Control::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	if ((Form::GetControlSelected() == this) && (!m_preventChangeControl)) {
		Point thisP1(GetAbsoluteLeft(), GetAbsoluteTop());
		Point thisP2(GetAbsoluteRight(), GetAbsoluteBottom());;
		Point thisCenter((thisP1.GetX() + thisP2.GetX()) / 2.0, (thisP1.GetY() + thisP2.GetY()) / 2.0);

		float distance = 30000;
		float distance2 = 30000;
		Control * selected = NULL;

		ArrayList list;
		GetTopParent()->GetControlsSelectables(&list);

		switch (button) {
			case RemoteButtons::Left:
				for (int i = 0; i<list.GetCount(); i++) {
					Control * control = (Control *)list.Get(i);
					if (control == this) continue;

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());;
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) / 2.0, (controlP1.GetY() + controlP2.GetY()) / 2.0);

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
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) / 2.0, (controlP1.GetY() + controlP2.GetY()) / 2.0);

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

				if (!selectedAux) break;
				distance = 30000;

				// Ahora busco de ese cercano, uno que este en la misma horizontal y mas cerca del actual
				// Y que ademas este por debajo del actual
				for (int i = 0; i<list.GetCount(); i++) {
					Control * control = (Control *)list.Get(i);
					if (control == this) continue;

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());;
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) / 2.0, (controlP1.GetY() + controlP2.GetY()) / 2.0);

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

				if (!selectedAux) break;
				distance = 30000;

				// Ahora busco de ese cercano, uno que este en la misma horizontal y mas cerca del actual
				// Y ademas que este por arriba del actual
				for (int i = 0; i<list.GetCount(); i++) {
					Control * control = (Control *)list.Get(i);
					if (control == this) continue;

					Point controlP1(control->GetAbsoluteLeft(), control->GetAbsoluteTop());
					Point controlP2(control->GetAbsoluteRight(), control->GetAbsoluteBottom());;
					Point controlCenter((controlP1.GetX() + controlP2.GetX()) / 2.0, (controlP1.GetY() + controlP2.GetY()) / 2.0);

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
	}

	return false;
}

bool Control::OnKeyPress(Keys::Enum key) {
	return false;
}

void Control::GetControlsSelectables(ArrayList * list) {
	for (int i = 0; i<GetControls()->GetCount(); i++) {
		Control * control = (Control *)GetControls()->Get(i);
		control->GetControlsSelectables(list);
	}

	if (GetTabStop())
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
