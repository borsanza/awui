// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Spinner.h"
#include <awui/Drawing/Color.h>
#include <SDL_opengl.h>

using namespace awui::Windows::Forms::Statistics;

Spinner::Spinner() {
	this->position = 0;
}

Spinner::~Spinner() {
}

void Spinner::OnTick() {
	static int mode = 0;

	mode++;
	this->position = (mode/4%4);
}

void Spinner::DrawLine(int x, int y, int x2, int y2) {
	glBegin(GL_LINES);
	glVertex2f(x, y);
	glVertex2f(x2, y2);
	glVertex2f(x2, y2);
	glVertex2f(x, y);
	glEnd();
}

void Spinner::OnPaint(OpenGL::GL * gl) {
	int size = 15;
	int left = (this->GetWidth() - size) / 2;
	int top = (this->GetHeight() - size) / 2;
	int right = left + size - 1;
	int bottom = top + size - 1;
	Drawing::Color color = this->GetForeColor();
	glColor4f(color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);
	switch (this->position) {
		case 0:
			this->DrawLine(left, top, right, top);
			break;
		case 1:
			this->DrawLine(left, top, left, bottom);
			break;
		case 2:
			this->DrawLine(left, bottom, right, bottom);
			break;
		case 3:
			this->DrawLine(right, top, right, bottom);
			break;
	}
}
