/*
 * awui/Windows/Emulators/DebuggerSMS.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "DebuggerSMS.h"

#include <awui/Emulation/MasterSystem/CPU.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Emulators/MasterSystem.h>
#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::Emulation::MasterSystem;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;

DebuggerSMS::DebuggerSMS(MasterSystem * sms) {
	this->_sms = sms;
}

DebuggerSMS::~DebuggerSMS() {
}

int DebuggerSMS::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::DebuggerSMS)
		return 1;

	return Control::IsClass(objectClass);
}

void DebuggerSMS::OnTick() {
}

void DebuggerSMS::OnPaint(OpenGL::GL * gl) {
	VDP * vdp = this->_sms->GetCPU()->GetVDP();
	uint8_t * colors = vdp->GetColors();

	for (int i=0; i<32; i++) {
		uint8_t b = (colors[i] >> 4) & 0x3;
		uint8_t g = (colors[i] >> 2) & 0x3;
		uint8_t r = colors[i] & 0x3;
		glColor4f(r / 3.0f, g / 3.0f, b / 3.0f, 1.0f);
		GL::FillRectangle((i * 6) + 1, 1, (((i + 1) * 6) - 1) + 1, 50);
	}
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	GL::DrawRectangle(0, 0, 193, 51);
}