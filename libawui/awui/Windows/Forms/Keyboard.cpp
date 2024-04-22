/*
 * awui/Windows/Forms/Keyboard.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Keyboard.h"

#include <awui/Core/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/Button.h>
#include <awui/Windows/Forms/Form.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Keyboard::Keyboard() {
	m_class = Classes::Keyboard;
	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	int pos = 0;
	const char *letras = "abcdefghijklmnopqrstuvwxyz1234567890";

	Button *focused = nullptr;
	Font font = Font("sans-serif", 34, FontStyle::Bold);
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			Button *button = new Button();
			button->SetDock(DockStyle::None);
			button->SetLocation(30 + j * 70, 30 + i * 55);
			button->SetSize(50, 50);
			button->SetFont(font);
			button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
			button->SetForeColor(Color::FromArgb(255, 255, 255));
			char frase[2];
			frase[0] = letras[pos];
			frase[1] = 0;
			button->SetText(frase);
			AddWidget(button);

			if (!focused) {
				focused = button;
			}

			pos++;
		}
	}

	if (focused) {
		focused->SetFocus();
	}

	font = Font("sans-serif", 25, FontStyle::Bold);

	Button *button = new Button();
	button->SetDock(DockStyle::None);
	button->SetLocation(30, 360);
	button->SetSize(120, 50);
	button->SetFont(font);
	button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	button->SetForeColor(Color::FromArgb(255, 255, 255));
	button->SetText("SPACE");
	AddWidget(button);

	button = new Button();
	button->SetDock(DockStyle::None);
	button->SetLocation(170, 360);
	button->SetSize(120, 50);
	button->SetFont(font);
	button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	button->SetForeColor(Color::FromArgb(255, 255, 255));
	button->SetText("DELETE");
	AddWidget(button);

	button = new Button();
	button->SetDock(DockStyle::None);
	button->SetLocation(310, 360);
	button->SetSize(120, 50);
	button->SetFont(font);
	button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	button->SetForeColor(Color::FromArgb(255, 255, 255));
	button->SetText("CLEAR");
	AddWidget(button);
}

bool Keyboard::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Keyboard) || Control::IsClass(objectClass);
}
