/**
 * awui/Windows/Emulators/DebuggerSMS.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "DebuggerSMS.h"

#include <SDL_opengl.h>
#include <awui/Drawing/Image.h>
#include <awui/Emulation/Common/Ram.h>
#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Emulators/MasterSystem.h>

using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;

DebuggerSMS::DebuggerSMS() {
	m_class = Classes::DebuggerSMS;
	m_width = 0;
	m_rom = NULL;
	m_tiles = new Drawing::Image(128, 256);
	m_colors = new Drawing::Image(32, 1);
	m_show = true;
}

bool DebuggerSMS::IsClass(Classes objectClass) const {
	return (objectClass == Classes::DebuggerSMS) || Button::IsClass(objectClass);
}

void DebuggerSMS::OnTick(float deltaSeconds) {
	int newWidth = GetWidth();
	if (!m_show) {
		if (m_width != 1) {
			m_width = m_width + ((0.0f - m_width) * 0.25f);
			if (m_width < 1)
				m_width = 1;
			newWidth = m_width;
		}
	} else {
		m_width = m_width + ((194.0f - m_width) * 0.25f);
		newWidth = m_width;
	}

	if (newWidth != GetWidth()) {
		SetWidth(newWidth);
		GetParent()->Layout();
	}

	if (m_width == 1)
		return;

	if (!m_rom)
		return;

	uint8_t c, r, g, b;
	uint8_t color[4]{0, 85, 170, 255};

	VDP *vdp = m_rom->GetCPU()->GetVDP();
	const uint8_t *colors = vdp->GetColors();

	for (int i = 0; i < 32; i++) {
		c = colors[i];
		r = color[c & 0x3];
		g = color[(c >> 2) & 0x3];
		b = color[(c >> 4) & 0x3];
		m_colors->SetPixel(i, 0, r, g, b);
	}

	m_colors->Update();

	uint8_t *ram = vdp->GetVram();

	for (int sprite = 0; sprite < 512; sprite++) {
		int64_t offset2 = sprite * 32;
		for (int y = 0; y < 8; y++) {
			int64_t offset = offset2 + (y * 4);
			uint8_t byte1 = ram[offset + 0];
			uint8_t byte2 = ram[offset + 1];
			uint8_t byte3 = ram[offset + 2];
			uint8_t byte4 = ram[offset + 3];

			// byte1 = byte2 = byte3 = byte4 = ((y == 0 || y == 7)? 0x81 : 0x00);
			uint8_t mask = 0x80;
			for (int x = 0; x < 8; x++) {
				uint8_t c;
				c = (byte1 & mask) >> (7 - x);
				c |= (((byte2 & mask) >> (7 - x)) << 1);
				c |= (((byte3 & mask) >> (7 - x)) << 2);
				c |= (((byte4 & mask) >> (7 - x)) << 3);
				c = colors[c];
				r = color[c & 0x3];
				g = color[(c >> 2) & 0x3];
				b = color[(c >> 4) & 0x3];

				int ox = ((sprite % 16) * 8) + x;
				int oy = ((sprite >> 4) * 8) + y;

				m_tiles->SetPixel(ox, oy, r, g, b);

				mask = mask >> 1;
			}
		}
	}

	m_tiles->Update();
}

void DebuggerSMS::OnPaint(OpenGL::GL *gl) {
	if (m_width == 1)
		return;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	GL::DrawRectangle(0, 0, 193, 51);
	GL::DrawImageGL(m_colors, 1, 1, 192, 50);

	int left = (GetWidth() - 128) >> 1;
	int top = 100;
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	GL::DrawRectangle(left - 1, top - 1, left + 128, top + 256);
	GL::DrawImageGL(m_tiles, left, top);
}

void DebuggerSMS::SetRom(MasterSystem *rom) {
	m_rom = rom;
}
