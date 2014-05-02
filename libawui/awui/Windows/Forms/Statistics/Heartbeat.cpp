// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Heartbeat.h"
#include <awui/Drawing/Color.h>
#include <awui/OpenGL/GL.h>
#include <SDL_opengl.h>

using namespace awui::OpenGL;
using namespace awui::Windows::Forms::Statistics;

Heartbeat::Heartbeat() {
	this->heartbeat = 0;
	this->SetWidth(24);
}

Heartbeat::~Heartbeat() {
}

void Heartbeat::OnPaint(OpenGL::GL * gl) {
	int size = 5;
	int left = (this->GetWidth() - size) / 2;
	int top = (this->GetHeight() - size) / 2;
	int right = left + size - 1;
	int bottom = top + size - 1;
	Drawing::Color color = this->GetForeColor();
	glColor4f(color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);

	if (this->heartbeat > 0) {
		GL::DrawRectangle(left, top, right, bottom);
		this->heartbeat--;
	}
}

void Heartbeat::OnRemoteHeartbeat() {
	this->heartbeat = 2;
}
