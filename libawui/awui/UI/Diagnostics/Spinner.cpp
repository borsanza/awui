// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Spinner.h"

#include <SDL_opengl.h>
#include <awui/Drawing/Color.h>
#include <awui/OpenGL/GL.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::UI::Diagnostics;

Spinner::Spinner() {
	m_position = 0;
	SetWidth(36);
}

Spinner::~Spinner() {
}

void Spinner::OnTick(float deltaSeconds) {
	static int mode = 0;

	mode++;
	m_position = (mode / 4 % 4);
}

void Spinner::OnPaint(OpenGL::GL *gl) {
	int size = 15;
	int left = (GetWidth() - size) / 2;
	int top = (GetHeight() - size) / 2;
	int right = left + size - 1;
	int bottom = top + size - 1;
	Color color = GetForeColor();

	switch (m_position) {
		case 0:
			GL::DrawLine(left, top, right, top, color);
			break;
		case 1:
			GL::DrawLine(left, top, left, bottom, color);
			break;
		case 2:
			GL::DrawLine(left, bottom, right, bottom, color);
			break;
		case 3:
			GL::DrawLine(right, top, right, bottom, color);
			break;
	}
}
