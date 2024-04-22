// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Heartbeat.h"

#include <SDL_opengl.h>
#include <awui/Core/Color.h>
#include <awui/OpenGL/GL.h>

using namespace awui::OpenGL;
using namespace awui::Windows::Forms::Statistics;

Heartbeat::Heartbeat() {
	this->heartbeat = false;
	this->SetWidth(24);
}

Heartbeat::~Heartbeat() {
}

void Heartbeat::OnPaint(OpenGL::GL *gl) {
	int size = 4;
	int left = (this->GetWidth() - size) / 2;
	int top = (this->GetHeight() - size) / 2;
	int right = left + size - 1;
	int bottom = top + size - 1;
	Color color = this->GetForeColor();
	glColor4ub(color.GetR(), color.GetG(), color.GetB(), color.GetA());

	if (this->heartbeat) {
		GL::DrawRectangle(left, top, right, bottom);
		this->heartbeat = false;
	}
}

void Heartbeat::OnRemoteHeartbeat() {
	this->heartbeat = true;
}
