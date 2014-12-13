/*
 * awui/Windows/Emulators/MasterSystem.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "MasterSystem.h"

#include <awui/Drawing/Image.h>
#include <awui/DateTime.h>
#include <awui/Emulation/MasterSystem/CPU.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <awui/Emulation/MasterSystem/SoundSDL.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Emulators/DebuggerSMS.h>

using namespace awui;
using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;
using namespace awui::Emulation::MasterSystem;

MasterSystem::MasterSystem() {
	this->SetSize(1, 1);
	this->_image = new Drawing::Image(1, 1);
	this->_cpu = new CPU();
	this->SetTabStop(true);
	this->_multiply = 1;
	this->_canChangeControl = true;
	this->_pause = false;

	this->_first = -1;
	this->_last = -1;
	this->_actual = -1;
	this->_lastTick = 0;

	for (int i = 0; i < TOTALSAVED; i++)
		this->_savedData[i] = (uint8_t *) calloc (CPUInst::GetSaveSize(), sizeof(uint8_t));
}

MasterSystem::~MasterSystem() {
	for (int i = 0; i < TOTALSAVED; i++)
		free(this->_savedData[i]);

	delete this->_cpu;
	delete this->_image;
}

int MasterSystem::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::MasterSystem)
		return 1;

	return Control::IsClass(objectClass);
}

void MasterSystem::LoadRom(const String file) {
	this->SetName(file);
	this->_cpu->LoadRom(file);
	this->_first = 0;
	this->_last = 0;
	this->_actual = 0;
	this->_lastTick = DateTime::GetNow().GetTicks();
	this->_cpu->SaveState(this->_savedData[this->_actual]);
}

void MasterSystem::CheckLimits() {
}

void MasterSystem::OnTick() {
	long long now = DateTime::GetNow().GetTicks();
	if ((now - this->_lastTick) > 10000000) {
		this->_lastTick = now;
		this->_actual++;
		if (this->_last < this->_actual)
			this->_last = this->_actual;
		this->_cpu->SaveState(this->_savedData[this->_actual % TOTALSAVED]);
	}

	this->_cpu->OnTick();
}

void MasterSystem::RunOpcode() {
	this->_cpu->RunOpcode();
}

CPU * MasterSystem::GetCPU() {
	return this->_cpu;
}

void MasterSystem::OnPaint(GL* gl) {
	uint8_t c, r, g, b;
	uint8_t color[4] {0, 85, 170, 255};
	VDP * screen = this->_cpu->GetVDP();

//	¿Lo rellenamos con el registro 7?
//	c = screen->GetBackColor();
//	r = color[c & 0x3];
//	g = color[(c >> 2) & 0x3];
//	b = color[(c >> 4) & 0x3];
//	this->SetBackColor(Color::FromArgb(255, r, g, b));

	int width = screen->GetVisualWidth();
	int height = screen->GetVisualHeight();

	if ((width != this->_image->GetWidth()) || (height != this->_image->GetHeight())) {
		delete this->_image;
		this->_image = new Drawing::Image(width, height);
		this->SetSize(width * this->_multiply, height * this->_multiply);
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			c = screen->GetPixel(x, y);
			r = color[c & 0x3];
			g = color[(c >> 2) & 0x3];
			b = color[(c >> 4) & 0x3];
			this->_image->SetPixel(x, y, r, g, b);
		}
	}

	this->_image->Update();

	GL::DrawImageGL(this->_image, 0, 0, this->GetWidth(), this->GetHeight());
}

bool MasterSystem::OnKeyPress(Keys::Enum key) {
	if (key == Keys::Key_B) {
		VDP * screen = this->_cpu->GetVDP();
		screen->SetShowBorder(!screen->GetShowBorder());
		screen->Clear();;
	}

	if (key == Keys::Key_D)
		this->_debugger->SetShow(!this->_debugger->GetShow());

	if (key == Keys::Key_BACKSPACE)
		this->_cpu->Reset();

	return true;
}

void MasterSystem::SetMultiply(int multiply) {
	this->_multiply = multiply;
}

uint32_t MasterSystem::GetCRC32() {
	return this->_cpu->GetCRC32();
}

uint8_t MasterSystem::GetPad(int which) const {
	if (this->_canChangeControl)
		return 0xFF;

	uint32_t buttons;
	switch (which) {
		default:
		case 0:
			buttons = Form::GetButtonsPad1();
			break;
		case 1:
			buttons = Form::GetButtonsPad2();
			break;
	}

	uint8_t pad = 0x00;

	if (!(buttons & RemoteButtons::Up))
		pad |= 0x01;

	if (!(buttons & RemoteButtons::Down))
		pad |= 0x02;

	if (!(buttons & RemoteButtons::Left))
		pad |= 0x04;

	if (!(buttons & RemoteButtons::Right))
		pad |= 0x08;

	if (!(buttons & RemoteButtons::Ok))
		pad |= _invertKeys ? 0x20 : 0x10;

	if (!(buttons & RemoteButtons::Play))
		pad |= _invertKeys ? 0x10 : 0x20;

	if (!(buttons & RemoteButtons::Pause))
		pad |= 0x40;

	return pad;
}

bool MasterSystem::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	bool ret = false;
	if (button & RemoteButtons::Button5) {
		this->_lastTick = DateTime::GetNow().GetTicks();
		this->_actual--;
		if (this->_actual < this->_first)
			this->_actual = this->_first;
		this->_cpu->LoadState(this->_savedData[this->_actual % TOTALSAVED]);
		ret = true;
	}

	if (button & RemoteButtons::Button6) {
		this->_lastTick = DateTime::GetNow().GetTicks();
		this->_actual++;
		if (this->_actual > this->_last)
			this->_actual = this->_last;
		this->_cpu->LoadState(this->_savedData[this->_actual % TOTALSAVED]);
		ret = true;
	}

	if ((Form::GetButtonsPad1() & RemoteButtons::Button5) && (button & RemoteButtons::Button6)) {
		this->_cpu->Reset();
		ret = true;
	}

	uint8_t pad1 = this->GetPad(0);
	uint8_t pad2 = this->GetPad(1);
	this->_cpu->SetPad1(pad1);
	this->_cpu->SetPad2(pad2);

	if (ret)
		return ret;

	bool paused = (((pad1 & 0x40) == 0) | ((pad2 & 0x40) == 0));
	if (paused) {
		if (!this->_pause) {
			this->_pause = true;
			this->_cpu->CallPaused();
		}
	}

	if (button & RemoteButtons::Button4)
		this->_debugger->SetShow(!this->_debugger->GetShow());

	if (this->_canChangeControl)
		return Control::OnRemoteKeyPress(which, button);

	return true;
}

bool MasterSystem::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	if ((button & RemoteButtons::Pause) || (button & RemoteButtons::Ok) || (button & RemoteButtons::Play))
		this->_canChangeControl = false;

	if (button & RemoteButtons::Menu)
		this->_canChangeControl = true;

	uint8_t pad1 = this->GetPad(0);
	uint8_t pad2 = this->GetPad(1);
	this->_cpu->SetPad1(pad1);
	this->_cpu->SetPad2(pad2);
	bool paused = (((pad1 & 0x40) == 0) | ((pad2 & 0x40) == 0));
	if (!paused)
		this->_pause = false;

	if (this->_canChangeControl)
		return Control::OnRemoteKeyUp(which, button);

	return true;
}

void MasterSystem::SetSoundEnabled(bool mode) {
	SoundSDL::Instance()->SetCPU(this->_cpu);
}
