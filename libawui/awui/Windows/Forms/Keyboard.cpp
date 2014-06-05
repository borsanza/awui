/*
 * awui/Windows/Forms/Keyboard.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Keyboard.h"

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/Button.h>
#include <awui/Windows/Forms/ControlCollection.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Keyboard::Keyboard() {
	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	int pos = 0;
	const char * letras = "abcdefghijklmnopqrstuvwxyz1234567890";

//	Font font = Font("Monospace", 34, FontStyle::Bold);
	Font font = Font("sans-serif", 34, FontStyle::Bold);
	for (int i=0; i<6; i++) {
		for (int j=0; j<6; j++) {
			Button * button = new Button();
			button->SetDock(DockStyle::None);
			button->SetLocation(30 + j * 70, 30 + i * 55);
			button->SetSize(50, 50);
			button->SetFont(&font);
			button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
			button->SetForeColor(Color::FromArgb(255, 255, 255));
			char frase[2];
			frase[0] = letras[pos];
			frase[1] = 0;
			button->SetText(frase);
			this->GetControls()->Add(button);
			pos++;
		}
	}

	font = Font("sans-serif", 25, FontStyle::Bold);

	Button * button = new Button();
	button->SetDock(DockStyle::None);
	button->SetLocation(30, 360);
	button->SetSize(120, 50);
	button->SetFont(&font);
	button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	button->SetForeColor(Color::FromArgb(255, 255, 255));
	button->SetText("SPACE");
	this->GetControls()->Add(button);

	button = new Button();
	button->SetDock(DockStyle::None);
	button->SetLocation(170, 360);
	button->SetSize(120, 50);
	button->SetFont(&font);
	button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	button->SetForeColor(Color::FromArgb(255, 255, 255));
	button->SetText("DELETE");
	this->GetControls()->Add(button);

	button = new Button();
	button->SetDock(DockStyle::None);
	button->SetLocation(310, 360);
	button->SetSize(120, 50);
	button->SetFont(&font);
	button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	button->SetForeColor(Color::FromArgb(255, 255, 255));
	button->SetText("CLEAR");
	this->GetControls()->Add(button);

}

Keyboard::~Keyboard() {
}

int Keyboard::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Keyboard)
		return 1;

	return Control::IsClass(objectClass);
}