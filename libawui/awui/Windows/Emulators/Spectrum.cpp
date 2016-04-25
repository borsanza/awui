/*
 * awui/Windows/Emulators/Spectrum.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Spectrum.h"

#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Drawing/Image.h>
#include <awui/DateTime.h>
#include <awui/Emulation/Spectrum/Motherboard.h>
#include <awui/Emulation/Spectrum/ULA.h>
#include <awui/IO/FileStream.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Emulators/DebuggerSMS.h>

using namespace awui::IO;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;
using namespace awui::Emulation::Spectrum;

Spectrum::Spectrum() {
	this->SetSize(1, 1);
	this->_image = new Drawing::Image(1, 1);
	this->_cpu = new Motherboard();
	this->SetTabStop(true);
	this->_multiply = 1;
	this->_canChangeControl = true;
	this->_pause = false;

	this->_first = -1;
	this->_last = -1;
	this->_lastTick = 0;
	this->_fileSlot = 0;

	this->_colors[0]  = 0x000000;
	this->_colors[1]  = 0x0000BF;
	this->_colors[2]  = 0xBF0000;
	this->_colors[3]  = 0xBF00BF;
	this->_colors[4]  = 0x00BF00;
	this->_colors[5]  = 0x00BFBF;
	this->_colors[6]  = 0xBFBF00;
	this->_colors[7]  = 0xBFBFBF;
	this->_colors[8]  = 0x000000;
	this->_colors[9]  = 0x0000FF;
	this->_colors[10] = 0xFF0000;
	this->_colors[11] = 0xFF00FF;
	this->_colors[12] = 0x00FF00;
	this->_colors[13] = 0x00FFFF;
	this->_colors[14] = 0xFFFF00;
	this->_colors[15] = 0xFFFFFF;
}

Spectrum::~Spectrum() {
	delete this->_cpu;
	delete this->_image;
}

int Spectrum::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Spectrum)
		return 1;

	return Control::IsClass(objectClass);
}

void Spectrum::LoadRom(const String file) {
	this->SetName(file);
	this->_cpu->LoadRom(file);
	this->_first = 0;
	this->_last = 0;
	this->_lastTick = DateTime::GetNow().GetTicks();
}

void Spectrum::CheckLimits() {
}

void Spectrum::OnTick() {
	this->_cpu->OnTick();
}

Motherboard * Spectrum::GetCPU() {
	return this->_cpu;
}

// Interface:
// 256x192: 1800 = 6144 bytes = (32 x 8) x 192 bits
// 300 = 768 bytes = 32 x 24 x 8 bits
// 4000-57FF Spectrum bitmap
// 5800-5AFF Spectrum attributes
// 7000 attribute lookup: 256 bytes.  64 colors of (paper, ink)
// 7100 pixel stretch, 16 bytes.

void Spectrum::OnPaint(GL* gl) {
	uint8_t c;
	ULA * screen = this->_cpu->GetULA();

	int width = screen->GetTotalWidth();
	int height = screen->GetTotalHeight();

	if ((width != this->_image->GetWidth()) || (height != this->_image->GetHeight())) {
		delete this->_image;
		this->_image = new Drawing::Image(width, height);
		this->SetSize(width * this->_multiply, height * this->_multiply);
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			c = screen->GetPixel(x, y);
			uint32_t color = _colors[c];
			this->_image->SetPixel(x, y, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
		}
	}

	this->_image->Update();

	GL::DrawImageGL(this->_image, 0, 0, this->GetWidth(), this->GetHeight());
}

void Spectrum::SetMultiply(int multiply) {
	this->_multiply = multiply;
}

void Spectrum::CallKey(int key, bool pressed) {
	if (pressed)
		this->_cpu->OnKeyPress(key / 10, 1 << (key % 10));
	else
		this->_cpu->OnKeyUp(key / 10, 1 << (key % 10));
}

void Spectrum::DoKey(Keys::Enum key, bool pressed) {
	switch (key) {
		case Keys::Key_LSHIFT:
		case Keys::Key_RSHIFT: this->CallKey(00, pressed); break;
		case Keys::Key_Z:      this->CallKey(01, pressed); break;
		case Keys::Key_X:      this->CallKey(02, pressed); break;
		case Keys::Key_C:      this->CallKey(03, pressed); break;
		case Keys::Key_V:      this->CallKey(04, pressed); break;
		case Keys::Key_A:      this->CallKey(10, pressed); break;
		case Keys::Key_S:      this->CallKey(11, pressed); break;
		case Keys::Key_D:      this->CallKey(12, pressed); break;
		case Keys::Key_F:      this->CallKey(13, pressed); break;
		case Keys::Key_G:      this->CallKey(14, pressed); break;
		case Keys::Key_Q:      this->CallKey(20, pressed); break;
		case Keys::Key_W:      this->CallKey(21, pressed); break;
		case Keys::Key_E:      this->CallKey(22, pressed); break;
		case Keys::Key_R:      this->CallKey(23, pressed); break;
		case Keys::Key_T:      this->CallKey(24, pressed); break;
		case Keys::Key_KP1:
		case Keys::Key_1:      this->CallKey(30, pressed); break;
		case Keys::Key_KP2:
		case Keys::Key_2:      this->CallKey(31, pressed); break;
		case Keys::Key_KP3:
		case Keys::Key_3:      this->CallKey(32, pressed); break;
		case Keys::Key_KP4:
		case Keys::Key_4:      this->CallKey(33, pressed); break;
		case Keys::Key_KP5:
		case Keys::Key_5:      this->CallKey(34, pressed); break;
		case Keys::Key_KP0:
		case Keys::Key_0:      this->CallKey(40, pressed); break;
		case Keys::Key_KP9:
		case Keys::Key_9:      this->CallKey(41, pressed); break;
		case Keys::Key_KP8:
		case Keys::Key_8:      this->CallKey(42, pressed); break;
		case Keys::Key_KP7:
		case Keys::Key_7:      this->CallKey(43, pressed); break;
		case Keys::Key_KP6:
		case Keys::Key_6:      this->CallKey(44, pressed); break;
		case Keys::Key_P:      this->CallKey(50, pressed); break;
		case Keys::Key_O:      this->CallKey(51, pressed); break;
		case Keys::Key_I:      this->CallKey(52, pressed); break;
		case Keys::Key_U:      this->CallKey(53, pressed); break;
		case Keys::Key_Y:      this->CallKey(54, pressed); break;
		case Keys::Key_KP_ENTER:
		case Keys::Key_ENTER:  this->CallKey(60, pressed); break;
		case Keys::Key_L:      this->CallKey(61, pressed); break;
		case Keys::Key_K:      this->CallKey(62, pressed); break;
		case Keys::Key_J:      this->CallKey(63, pressed); break;
		case Keys::Key_H:      this->CallKey(64, pressed); break;
		case Keys::Key_SPACE:  this->CallKey(70, pressed); break;
		case Keys::Key_LALT:   this->CallKey(71, pressed); break;
		case Keys::Key_M:      this->CallKey(72, pressed); break;
		case Keys::Key_N:      this->CallKey(73, pressed); break;
		case Keys::Key_B:      this->CallKey(74, pressed); break;

		case Keys::Key_COMMA:
			this->CallKey(71, pressed);
			this->CallKey(73, pressed);
			break;

		case Keys::Key_QUOTE:
			this->CallKey(71, pressed);
			this->CallKey(43, pressed);
			break;

		case Keys::Key_KP_MINUS:
		case Keys::Key_MINUS:
			this->CallKey(71, pressed);
			this->CallKey(63, pressed);
			break;

		case Keys::Key_KP_PLUS:
			this->CallKey(71, pressed);
			this->CallKey(62, pressed);
			break;

		case Keys::Key_KP_DIVIDE:
			this->CallKey(71, pressed);
			this->CallKey(04, pressed);
			break;

		case Keys::Key_KP_MULTIPLY:
			this->CallKey(71, pressed);
			this->CallKey(74, pressed);
			break;

		case Keys::Key_KP_PERIOD:
		case Keys::Key_PERIOD:
			this->CallKey(71, pressed);
			this->CallKey(72, pressed);
			break;

		case Keys::Key_BACKSPACE:
			this->CallKey(00, pressed);
			this->CallKey(40, pressed);
			break;

		case Keys::Key_F2:
			this->CallKey(00, pressed);
			this->CallKey(30, pressed);
			break;

		default: break;
	}
}

bool Spectrum::OnKeyPress(Keys::Enum key) {
	this->DoKey(key, true);
	return true;
}

bool Spectrum::OnKeyUp(Keys::Enum key) {
	this->DoKey(key, false);
	return true;
}

uint8_t Spectrum::GetPad(int which) const {
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

bool Spectrum::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	bool ret = false;

	//Console::WriteLine(Convert::ToString(button));
/*
	if (Form::GetButtonsPad1() == RemoteButtons::SNES_L) {
		this->_lastTick = DateTime::GetNow().GetTicks();
		this->_actual--;
		if (this->_actual < this->_first)
			this->_actual = this->_first;
		this->_cpu->LoadState(this->_savedData[this->_actual % TOTALSAVED]);
		ret = true;
	}
*/
/*
	if (Form::GetButtonsPad1() == RemoteButtons::SNES_R) {
		this->_lastTick = DateTime::GetNow().GetTicks();
		this->_actual++;
		if (this->_actual > this->_last)
			this->_actual = this->_last;
		this->_cpu->LoadState(this->_savedData[this->_actual % TOTALSAVED]);
		ret = true;
	}
*/
	if (_fileSlot > 0 && (Form::GetButtonsPad1() == RemoteButtons::SPECIAL_SLOT_DECREASE)) {
		_fileSlot--;
		Console::Write("Estado: ");
		Console::WriteLine(Convert::ToString(_fileSlot));
	}

	if (Form::GetButtonsPad1() == RemoteButtons::SPECIAL_SLOT_INCREASE) {
		_fileSlot++;
		Console::Write("Estado: ");
		Console::WriteLine(Convert::ToString(_fileSlot));
	}

	if (Form::GetButtonsPad1() == RemoteButtons::SPECIAL_LOAD) {
		String name = "file.state";
		if (_fileSlot > 0)
			name = String::Concat(name, Convert::ToString(_fileSlot));

		if (File::Exists(name)) {
			Console::Write("Cargando: ");
			Console::WriteLine(name);

			uint8_t * savedData = (uint8_t *) calloc (Motherboard::GetSaveSize(), sizeof(uint8_t));
			FileStream * file = new FileStream(name, FileMode::Open, FileAccess::Read);
			for (unsigned int i = 0; i < file->GetLength(); i++)
				savedData[i] = file->ReadByte();

			this->_cpu->LoadState(savedData);
			free(savedData);
		}
	}

	if (Form::GetButtonsPad1() == RemoteButtons::SPECIAL_SAVE) {
		String name = "file.state";
		if (_fileSlot > 0)
			name = String::Concat(name, Convert::ToString(_fileSlot));

		Console::Write("Guardando: ");
		Console::WriteLine(name);

		uint8_t * savedData = (uint8_t *) calloc (Motherboard::GetSaveSize(), sizeof(uint8_t));
		FileStream * file = new FileStream(name, FileMode::Truncate, FileAccess::Write);
		this->_cpu->SaveState(savedData);

		for (int i = 0; i < Motherboard::GetSaveSize(); i++)
			file->WriteByte(savedData[i]);

		file->Close();
		delete file;

		free(savedData);
	}


	if (Form::GetButtonsPad1() == RemoteButtons::SPECIAL_RESET) {
		this->_cpu->Reset();
		ret = true;
	}

//	uint8_t pad1 = this->GetPad(0);
//	uint8_t pad2 = this->GetPad(1);
//	this->_cpu->SetPad1(pad1);
//	this->_cpu->SetPad2(pad2);

	if (ret)
		return ret;

	if (this->_canChangeControl)
		return Control::OnRemoteKeyPress(which, button);
	else {
		switch (button) {
			case RemoteButtons::Up:
				this->CallKey(00, true);
				this->CallKey(43, true);
				break;
			case RemoteButtons::Down:
				this->CallKey(00, true);
				this->CallKey(44, true);
				break;
			case RemoteButtons::Left:
				this->CallKey(00, true);
				this->CallKey(34, true);
				break;
			case RemoteButtons::Right:
				this->CallKey(00, true);
				this->CallKey(42, true);
				break;
			default:
				break;
		}
	}

	return true;
}

bool Spectrum::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	if ((button & RemoteButtons::Pause) || (button & RemoteButtons::Ok) || (button & RemoteButtons::Play))
		this->_canChangeControl = false;

	if (button & RemoteButtons::Menu)
		this->_canChangeControl = true;

	uint8_t pad1 = this->GetPad(0);
	uint8_t pad2 = this->GetPad(1);
//	this->_cpu->SetPad1(pad1);
//	this->_cpu->SetPad2(pad2);
	bool paused = (((pad1 & 0x40) == 0) | ((pad2 & 0x40) == 0));
	if (!paused)
		this->_pause = false;

	if (this->_canChangeControl)
		return Control::OnRemoteKeyUp(which, button);
	else {
		switch (button) {
			case RemoteButtons::Up:
				this->CallKey(00, false);
				this->CallKey(43, false);
				break;
			case RemoteButtons::Down:
				this->CallKey(00, false);
				this->CallKey(44, false);
				break;
			case RemoteButtons::Left:
				this->CallKey(00, false);
				this->CallKey(34, false);
				break;
			case RemoteButtons::Right:
				this->CallKey(00, false);
				this->CallKey(42, false);
				break;
			default:
				break;
		}
	}

	return true;
}
