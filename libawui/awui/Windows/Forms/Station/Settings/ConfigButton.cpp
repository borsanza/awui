/**
 * awui/Windows/Forms/Station/Settings/ConfigButton.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "ConfigButton.h"

#include <algorithm>
#include <SDL_opengl.h>
#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Events/IRemoteListener.h>
#include <awui/Windows/Forms/Events/MouseEventArgs.h>
#include <awui/Windows/Forms/Station/Page.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms::Events;
using namespace awui::Windows::Forms::Station::Settings;
using namespace awui::Windows::Forms::Input;

#define OFFSET 0.5f

ConfigButton::ConfigButton(TypeButton typeButton) {
	m_typeButton = typeButton;
	m_subpage = nullptr;
	m_boolValue = false;
	m_selected = -1;

	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	SetFont(Font("Liberation Sans", 28, FontStyle::Bold));
	SetDock(DockStyle::None);
	SetFocusable(true);

	m_value.SetTextAlign(ContentAlignment::MiddleRight);

	AddWidget(&m_label, WidgetOwnership::Borrowed);
	AddWidget(&m_value, WidgetOwnership::Borrowed);
}

ConfigButton::~ConfigButton() {
}

void ConfigButton::OnPaint(GL *gl) {
	Form *form = GetForm();
	if (form && (form->GetChildFocused() == this)) {
		SetForeColor(Color::FromArgb(255, 255, 255));
	} else {
		SetForeColor(Color::FromArgb(199, 199, 199));
	}

	if (IsGroup()) {
		glLineWidth(2.5f);

		float x = GetWidth() - 22.0f;
		float y = (GetHeight() / 2.0f) - 0.5f;

		Form *form = GetForm();
		if (form && (form->GetChildFocused() == this)) {
			glColor3ub(255, 255, 255);
		} else {
			glColor3ub(199, 199, 199);
		}

		glEnable(GL_LINE_SMOOTH);
		glBegin(GL_LINE_STRIP);
		glVertex2f(x - 10.0f + OFFSET, y - 10.0f + OFFSET);
		glVertex2f(x + OFFSET, y + OFFSET);
		glVertex2f(x - 10.0f + OFFSET, y + 10.0f + OFFSET);
		glEnd();
		glDisable(GL_LINE_SMOOTH);
	}

	// Marca de la opción elegida
	if ((m_typeButton == TypeButton::Option) && m_boolValue) {
		glLineWidth(3.0f);

		float x = GetWidth() - 22.0f;
		float y = (GetHeight() / 2.0f) - 0.5f;

		Form *form = GetForm();
		if (form && (form->GetChildFocused() == this)) {
			glColor3ub(255, 255, 255);
		} else {
			glColor3ub(199, 199, 199);
		}

		glEnable(GL_LINE_SMOOTH);
		glBegin(GL_LINE_STRIP);
		glVertex2f(x - 20.0f + OFFSET, y + OFFSET);
		glVertex2f(x - 12.0f + OFFSET, y + 8.0f + OFFSET);
		glVertex2f(x + OFFSET, y - 10.0f + OFFSET);
		glEnd();
		glDisable(GL_LINE_SMOOTH);
	}
}

void ConfigButton::SetText(const String str) {
	m_label.SetText(str);
}

void ConfigButton::SetForeColor(const Color color) {
	if (color != GetForeColor()) {
		Control::SetForeColor(color);
		m_label.SetForeColor(GetForeColor());
		m_value.SetForeColor(GetForeColor());
	}
}

const awui::String ConfigButton::GetText() const {
	return m_label.GetText();
}

void ConfigButton::SetFont(const Drawing::Font font) {
	Control::SetFont(font);
	m_label.SetFont(font);
	m_value.SetFont(font);
}

int ConfigButton::GetLabelWidth() const {
	return m_label.GetLabelWidth();
}

void ConfigButton::OnResize() {
	m_label.SetLocation(23, 0);
	if (IsGroup() || (m_typeButton == TypeButton::Option)) {
		m_label.SetSize(GetWidth() - (50 + m_label.GetLeft()), GetHeight());
		m_value.SetSize(0, GetHeight());
		return;
	}

	// El valor a la derecha, entero; el nombre ocupa el resto (y se desplaza si no cabe)
	int valueWidth = (m_value.GetText().GetLength() > 0) ? m_value.GetLabelWidth() + 2 : 0;
	m_value.SetLocation(GetWidth() - 23 - valueWidth, 0);
	m_value.SetSize(valueWidth, GetHeight());
	m_label.SetSize(GetWidth() - (m_label.GetLeft() + 23 + valueWidth + (valueWidth ? 30 : 0)), GetHeight());
}

void ConfigButton::SetValueText(const String &text) {
	m_value.SetText(text);
	OnResize();
}

void ConfigButton::UpdateValueText() {
	switch (m_typeButton) {
		case TypeButton::Boolean:
			SetValueText(m_boolValue ? m_onText : m_offText);
			break;
		case TypeButton::List:
			SetValueText((m_selected >= 0) ? m_options[m_selected].second : String(""));
			break;
		default:
			break;
	}
}

void ConfigButton::SetBoolValue(bool value, const String &onText, const String &offText) {
	m_boolValue = value;
	m_onText = onText;
	m_offText = offText;
	UpdateValueText();
}

void ConfigButton::SetOptions(const std::vector<std::pair<std::string, String>> &options, const std::string &selected) {
	m_options = options;
	m_selected = m_options.empty() ? -1 : 0;
	for (int i = 0; i < (int) m_options.size(); i++) {
		if (m_options[i].first == selected) {
			m_selected = i;
			break;
		}
	}

	UpdateValueText();
}

std::string ConfigButton::GetListValue() const {
	return (m_selected >= 0) ? m_options[m_selected].first : "";
}

void ConfigButton::SetListValue(const std::string &code) {
	for (int i = 0; i < (int) m_options.size(); i++) {
		if (m_options[i].first == code) {
			m_selected = i;
			UpdateValueText();
			return;
		}
	}
}

// Cambia el valor (sí/no alterna, las listas avanzan o retroceden de forma circular) y avisa con onValueChanged
void ConfigButton::Step(int direction) {
	switch (m_typeButton) {
		case TypeButton::Boolean:
			m_boolValue = !m_boolValue;
			break;
		case TypeButton::List: {
			int count = (int) m_options.size();
			if (count < 2)
				return;

			m_selected = (m_selected + direction + count) % count;
		} break;
		default:
			return;
	}

	UpdateValueText();
	if (m_onValueChanged)
		m_onValueChanged(this);
}

void ConfigButton::Click() {
	for (auto *listener : m_listeners) {
		listener->OnOk(this);
	}
}

void ConfigButton::AddOnClickListener(IRemoteListener *listener) {
	m_listeners.push_back(listener);
}

void ConfigButton::RemoveOnClickListener(IRemoteListener *listener) {
	m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), listener), m_listeners.end());
}

void ConfigButton::RemoveAllListeners() {
	m_listeners.clear();
}

void ConfigButton::OnMouseDown(MouseEventArgs *e) {
	switch (e->GetButton()) {
		case MouseButtons::Left: {
			if (!IsFocused()) {
				SetFocus();
			} else {
				OnRemoteKeyUp(0, RemoteButtons::Ok);
			}
		} break;
		case MouseButtons::Right:
			OnRemoteKeyUp(0, RemoteButtons::Menu);
			break;
		default:
			break;
	}
}

awui::String ConfigButton::ToString() const {
	String a = awui::String("awui::Windows::Forms::Station::Settings::ConfigButton (");
	a += GetText();
	a += awui::String(")");
	return a;
}

// Izquierda y derecha cambian el valor de los sí/no y las listas en vez de mover el foco
bool ConfigButton::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	if ((m_typeButton == TypeButton::Boolean) || (m_typeButton == TypeButton::List)) {
		switch (button) {
			case RemoteButtons::Left:
				Step(-1);
				return true;
			case RemoteButtons::Right:
				Step(1);
				return true;
			default:
				break;
		}
	}

	// Derecha también abre un grupo
	if (IsGroup() && (button == RemoteButtons::Right)) {
		Click();
		return true;
	}

	return Control::OnRemoteKeyPress(which, button);
}

bool ConfigButton::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	switch (button) {
		case RemoteButtons::Ok:
			switch (m_typeButton) {
				case TypeButton::Boolean:
					Step(1);
					break;
				case TypeButton::Group:
				case TypeButton::List:
				case TypeButton::Option:
					Click();
					break;
				default:
					break;
			}
			break;
		case RemoteButtons::Menu: {
			// Copia: el listener puede cerrar el menú
			std::vector<IRemoteListener *> listeners = m_listeners;
			for (auto *listener : listeners) {
				listener->OnMenu(this);
			}
		} break;
		default:
			break;
	}

	return 1;
}