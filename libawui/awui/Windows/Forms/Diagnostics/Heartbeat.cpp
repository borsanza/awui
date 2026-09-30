// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Heartbeat.h"

#include <SDL_opengl.h>
#include <awui/Drawing/Color.h>
#include <awui/OpenGL/GL.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms::Diagnostics;

Heartbeat::Heartbeat() {
	m_heartbeat = false;
	SetWidth(24);
}

Heartbeat::~Heartbeat() {
}

void Heartbeat::OnPaint(OpenGL::GL *gl) {
	int size = 4;
	int left = (GetWidth() - size) / 2;
	int top = (GetHeight() - size) / 2;
	int right = left + size - 1;
	int bottom = top + size - 1;
	Color color = GetForeColor();
	glColor4ub(color.GetR(), color.GetG(), color.GetB(), color.GetA());

	if (m_heartbeat) {
		GL::DrawRectangle(left, top, right, bottom);
		m_heartbeat = false;
	}
}

void Heartbeat::OnRemoteHeartbeat() {
	m_heartbeat = true;
}
