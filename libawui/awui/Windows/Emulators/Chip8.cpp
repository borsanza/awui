/**
 * awui/Windows/Emulators/Chip8.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Chip8.h"

#include <awui/Drawing/Image.h>
#include <awui/Emulation/Chip8/CPU.h>
#include <awui/Emulation/Chip8/Screen.h>
#include <awui/OpenGL/GL.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;

bool Chip8::m_invertedColors = false;

Chip8::Chip8() {
	m_class = Classes::Chip8;
	m_image = new Drawing::Image(64, 32);
	m_cpu = new CPU();
	m_lastInverted = Chip8::m_invertedColors;
}

Chip8::~Chip8() {
	delete m_cpu;
}

bool Chip8::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Chip8) || ArcadeContainer::IsClass(objectClass);
}

void Chip8::CheckBackcolor() {
	if (m_cpu->GetChip8Mode() == MEGACHIP8)
		SetBackColor(Color::FromArgb(0, 0, 0));
	else {
		if (!Chip8::m_invertedColors)
			SetBackColor(Color::FromArgb(163, 218, 2));
		else
			SetBackColor(Color::FromArgb(0, 0, 0));
	}
}

void Chip8::LoadRom(const String file) {
	m_cpu->LoadRom(file);
	SetName(file);

	m_cpu->OnTick(0.16f);
	CheckBackcolor();
}

void Chip8::OnTick(float deltaSeconds) {
	if (m_lastInverted != Chip8::m_invertedColors) {
		UpdateImage();
		m_lastInverted = Chip8::m_invertedColors;
	}

	m_cpu->OnTick(deltaSeconds);
}

void Chip8::UpdateImage() {
	Screen * screen = m_cpu->GetScreen();

	CheckBackcolor();

	if ((screen->GetWidth() != m_image->GetWidth()) || (screen->GetHeight() != m_image->GetHeight())) {
		delete m_image;
		m_image = new Drawing::Image(screen->GetWidth(), screen->GetHeight());
	}

	if (m_cpu->GetChip8Mode() == MEGACHIP8) {
		for (int y = 0; y < screen->GetHeight(); y++) {
			for (int x = 0; x < screen->GetWidth(); x++) {
				uint32_t pixel = screen->GetPixel(x, y);
				m_image->SetPixel(x, y, (pixel >> 16) & 0xFF, (pixel >> 8) & 0xFF, pixel & 0xFF, 255);
			}
		}
	} else {
		for (int y = 0; y < screen->GetHeight(); y++) {
			for (int x = 0; x < screen->GetWidth(); x++) {
				if (!Chip8::m_invertedColors) {
					if (screen->GetPixel(x, y))
						m_image->SetPixel(x, y, 50, 88, 4, 255);
					else
						m_image->SetPixel(x, y, 128, 185, 0, 255);
				} else {
					if (screen->GetPixel(x, y))
						m_image->SetPixel(x, y, 255, 255, 255, 255);
					else
						m_image->SetPixel(x, y, 0, 0, 0, 255);
				}
			}
		}
	}

	m_image->Update();
	m_cpu->SetImageUpdated(false);
}

void Chip8::OnPaint(GL* gl) {
	Screen * screen = m_cpu->GetScreen();

	if (m_cpu->GetImageUpdated())
		UpdateImage();

	int width = screen->GetWidth();
	int height = screen->GetHeight();

	float w = width;
	float h = height;

	float ratio = w / h;
	w = GetWidth();
	h = GetWidth() / ratio;

	if (h > GetHeight()) {
		h = GetHeight();
		w = GetHeight() * ratio;
	}

	int ratio2 = w / width;
	if (ratio2 >= 1) {
		w = width * ratio2;
		h = height * ratio2;
	}

	GL::DrawImageGL(m_image, int(GetWidth() - w) >> 1, int(GetHeight() - h) >> 1, w, h);
}

int Chip8::GetChip8Mode() const {
	return m_cpu->GetChip8Mode();
}

int Chip8::ConvertKeyAwToChip8(Keys::Enum key) {
	int keyPressed = -1;

	switch (key) {
		case Keys::Key_X:
		case Keys::Key_0:
			keyPressed = 0;
			break;

		case Keys::Key_1:
		case Keys::Key_KP7:
			keyPressed = 1;
			break;

		case Keys::Key_KP8:
		case Keys::Key_2:
			keyPressed = 2;
			break;

		case Keys::Key_3:
		case Keys::Key_KP9:
			keyPressed = 3;
			break;

		case Keys::Key_Q:
		case Keys::Key_KP4:
			keyPressed = 4;
			break;

		case Keys::Key_W:
		case Keys::Key_KP5:
		case Keys::Key_5:
			keyPressed = 5;
			break;

		case Keys::Key_E:
		case Keys::Key_KP6:
		case Keys::Key_6:
			keyPressed = 6;
			break;

		case Keys::Key_A:
		case Keys::Key_7:
		case Keys::Key_KP1:
			keyPressed = 7;
			break;

		case Keys::Key_S:
		case Keys::Key_KP2:
		case Keys::Key_8:
			keyPressed = 8;
			break;

		case Keys::Key_D:
		case Keys::Key_9:
		case Keys::Key_KP3:
			keyPressed = 9;
			break;

		case Keys::Key_Z:
			keyPressed = 10;
			break;

		case Keys::Key_C:
		case Keys::Key_B:
			keyPressed = 11;
			break;

		case Keys::Key_4:
			keyPressed = 12;
			break;

		case Keys::Key_R:
			keyPressed = 13;
			break;

		case Keys::Key_F:
			keyPressed = 14;
			break;

		case Keys::Key_V:
			keyPressed = 15;
			break;

		default:
			break;
	}

	return keyPressed;
}

int Chip8::ConvertRemoteKeyToChip8(RemoteButtons::Enum button) {
	int keyPressed = -1;

	switch (button) {
		case RemoteButtons::Up:
			keyPressed = 2;
			break;
		case RemoteButtons::Left:
			keyPressed = 4;
			break;
		case RemoteButtons::Right:
			keyPressed = 6;
			break;
		case RemoteButtons::Down:
			keyPressed = 8;
			break;
		case RemoteButtons::Ok:
			keyPressed = 5;
			break;
		default:
			break;
	}

	return keyPressed;
}

bool Chip8::OnKeyPress(Keys::Enum key) {
	int keypressed = ConvertKeyAwToChip8(key);
	if (keypressed >= 0)
		m_cpu->KeyDown(keypressed);

	if (key == Keys::Key_I)
		SetInvertedColors(!Chip8::m_invertedColors);

	return true;
}

bool Chip8::OnKeyUp(Keys::Enum key) {
	int keypressed = ConvertKeyAwToChip8(key);
	if (keypressed >= 0)
		m_cpu->KeyUp(keypressed);

	return true;
}

void Chip8::SetInvertedColors(bool mode) {
	Chip8::m_invertedColors = mode;
	UpdateImage();
}


bool Chip8::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	int keypressed = ConvertRemoteKeyToChip8(button);
	if (keypressed >= 0) {
		m_cpu->KeyDown(keypressed);
		return true;
	}

	return ArcadeContainer::OnRemoteKeyPress(which, button);
}

bool Chip8::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	int keypressed = ConvertRemoteKeyToChip8(button);
	if (keypressed >= 0) {
		m_cpu->KeyUp(keypressed);
		return true;
	}

	return ArcadeContainer::OnRemoteKeyUp(which, button);
}
