// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Spinner.h"

#include <SDL_opengl.h>
#include <awui/Core/Color.h>
#include <awui/OpenGL/GL.h>

using namespace awui::OpenGL;
using namespace awui::Windows::Forms::Statistics;

Spinner::Spinner() {
	this->position = 0;
	this->SetWidth(36);
}

Spinner::~Spinner() {
}

void Spinner::OnTick(float deltaSeconds) {
	static int mode = 0;

	mode++;
	this->position = (mode / 4 % 4);
}

void Spinner::OnPaint(OpenGL::GL *gl) {
	int size = 15;
	int left = (this->GetWidth() - size) / 2;
	int top = (this->GetHeight() - size) / 2;
	int right = left + size - 1;
	int bottom = top + size - 1;
	Color color = this->GetForeColor();
	glColor4ub(color.GetR(), color.GetG(), color.GetB(), color.GetA());

	switch (this->position) {
		case 0:
			GL::DrawLine(left, top, right, top);
			break;
		case 1:
			GL::DrawLine(left, top, left, bottom);
			break;
		case 2:
			GL::DrawLine(left, bottom, right, bottom);
			break;
		case 3:
			GL::DrawLine(right, top, right, bottom);
			break;
	}
}
