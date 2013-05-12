// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Keyboard.h"

#include <awui/Drawing/Color.h>
#include <awui/Convert.h>
#include <awui/Windows/Forms/Button.h>
#include <awui/Windows/Forms/ControlCollection.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Keyboard::Keyboard() {
	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	int pos = 0;
	for (int i=0; i<6; i++) {
		for (int j=0; j<6; j++) {
			Button * button = new Button();
			button->SetDock(DockStyle::None);
			button->SetLocation(j * 70, i * 50);
			button->SetSize(50, 50);
			button->SetFont(new Font("Monospace", 20, FontStyle::Bold)); // | FontStyle::Underline | FontStyle::Strikeout));
			button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
			button->SetForeColor(Color::FromArgb(255, 255, 255));
			int letra;
			if (pos < 26)
				letra = 'a' + pos;
			else
				letra = '0' + pos - 26;
			char frase[2];
			frase[0] = letra;
			frase[1] = 0;
			button->SetText(frase);
			this->GetControls()->Add(button);
			pos++;
		}
	}
}

Keyboard::~Keyboard() {
}

int Keyboard::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Keyboard)
		return 1;

	return Control::IsClass(objectClass);
}
