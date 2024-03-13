/**
 * awui/Windows/Emulators/Spectrum.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Spectrum.h"

#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/DateTime.h>
#include <awui/Drawing/Image.h>
#include <awui/Emulation/Spectrum/Motherboard.h>
#include <awui/Emulation/Spectrum/SoundSDL.h>
#include <awui/Emulation/Spectrum/TapeCorder.h>
#include <awui/Emulation/Spectrum/ULA.h>
#include <awui/IO/FileStream.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Form.h>

using namespace awui::Drawing;
using namespace awui::Emulation::Spectrum;
using namespace awui::IO;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;

void WriteCassetteCB(int32_t value, void * data) { /* printf("%d\n", value); */ }

int32_t ReadCassetteCB(void * data) {
	TapeCorder * tape = ((Spectrum *) data)->GetTapeCorder();
	if (!tape)
		return 0;

	return tape->GetNext();
}

void FinishCassetteCB(void * data) {
	Spectrum * spectrum = ((Spectrum *) data);
	if (spectrum && spectrum->GetMotherboard())
		spectrum->GetMotherboard()->SetFast(false);
}

Spectrum::Spectrum() {
	m_class = Classes::Spectrum;
	SetSize(1, 1);
	m_motherboard = new Motherboard();
	m_motherboard->SetWriteCassetteCB(WriteCassetteCB, this);
	m_motherboard->SetReadCassetteCB(ReadCassetteCB, this);

	m_pause = false;

	m_first = -1;
	m_last = -1;
	m_lastTick = 0;
	m_fileSlot = 0;
	m_tapecorder = new TapeCorder();
	m_tapecorder->SetFinishCassetteCB(FinishCassetteCB, this);
}

Spectrum::~Spectrum() {
	delete m_motherboard;
}

bool Spectrum::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Spectrum) || ArcadeContainer::IsClass(objectClass);
}

void Spectrum::LoadRom(const String file) {
	if (file.EndsWith(".rom"))
		m_motherboard->LoadRom(file);

	if (file.EndsWith(".tap")) {
		String rom = "roms/zxspectrum/48.rom";
		ArrayList list = file.Split("/");
		int found = -1;
		String system;
		for (int i = 0; i < list.GetCount(); i++) {
			String * name = (String *)list.Get(i);

			if (name->CompareTo("roms") == 0)
				found = i + 2;

			if (found == i)
				system = String::Concat(*name, ".rom");

			delete name;
		}

		if (found != -1)
			rom = String::Concat("roms/zxspectrum/", system);

		m_motherboard->LoadRom(rom);
		m_tapecorder->LoadFile(file);
	}

	m_first = 0;
	m_last = 0;
	m_lastTick = DateTime::GetNow().GetTicks();
}

void Spectrum::CheckLimits() {
}

void Spectrum::OnTick(float deltaSeconds) {
	if (Form::GetButtonsPad1() == RemoteButtons::SPECIAL_RESET)
		m_motherboard->Reset();

	m_motherboard->OnTick();
}

Motherboard * Spectrum::GetCPU() {
	return m_motherboard;
}

// Interface:
// 256x192: 1800 = 6144 bytes = (32 x 8) x 192 bits
// 300 = 768 bytes = 32 x 24 x 8 bits
// 4000-57FF Spectrum bitmap
// 5800-5AFF Spectrum attributes
// 7000 attribute lookup: 256 bytes.  64 colors of (paper, ink)
// 7100 pixel stretch, 16 bytes.

void Spectrum::OnPaint(GL* gl) {
	ULA * ula = m_motherboard->GetULA();

	int width = ula->GetImage()->GetWidth();
	int height = ula->GetImage()->GetHeight();


	//GL::DrawImageGL(ula->GetImage(), 0, 0, GetWidth(), GetHeight());

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

	GL::DrawImageGL(ula->GetImage(), int(GetWidth() - w) >> 1, int(GetHeight() - h) >> 1, w, h);
}

void Spectrum::CallKey(int key, bool pressed) {
	if (pressed)
		m_motherboard->OnKeyPress(key / 10, 1 << (key % 10));
	else
		m_motherboard->OnKeyUp(key / 10, 1 << (key % 10));
}

void Spectrum::DoKey(Keys::Enum key, bool pressed) {
	switch (key) {
		case Keys::Key_F2:     if (pressed) SaveState(); break;
		case Keys::Key_F4:     if (pressed) LoadState(); break;
		case Keys::Key_F8:     if (pressed) m_motherboard->SetFast(!m_motherboard->GetFast()); break;
		case Keys::Key_F9:
			if (pressed) {
				if (!m_tapecorder->IsPlaying()) {
					m_tapecorder->Rewind();
					m_tapecorder->Play();
				} else
					m_tapecorder->Stop();
			}
			break;
		case Keys::Key_LSHIFT:
		case Keys::Key_RSHIFT: CallKey(00, pressed); break;
		case Keys::Key_Z:      CallKey(01, pressed); break;
		case Keys::Key_X:      CallKey(02, pressed); break;
		case Keys::Key_C:      CallKey(03, pressed); break;
		case Keys::Key_V:      CallKey(04, pressed); break;
		case Keys::Key_A:      CallKey(10, pressed); break;
		case Keys::Key_S:      CallKey(11, pressed); break;
		case Keys::Key_D:      CallKey(12, pressed); break;
		case Keys::Key_F:      CallKey(13, pressed); break;
		case Keys::Key_G:      CallKey(14, pressed); break;
		case Keys::Key_Q:      CallKey(20, pressed); break;
		case Keys::Key_W:      CallKey(21, pressed); break;
		case Keys::Key_E:      CallKey(22, pressed); break;
		case Keys::Key_R:      CallKey(23, pressed); break;
		case Keys::Key_T:      CallKey(24, pressed); break;
		case Keys::Key_KP1:
		case Keys::Key_1:      CallKey(30, pressed); break;
		case Keys::Key_KP2:
		case Keys::Key_2:      CallKey(31, pressed); break;
		case Keys::Key_KP3:
		case Keys::Key_3:      CallKey(32, pressed); break;
		case Keys::Key_KP4:
		case Keys::Key_4:      CallKey(33, pressed); break;
		case Keys::Key_KP5:
		case Keys::Key_5:      CallKey(34, pressed); break;
		case Keys::Key_KP0:
		case Keys::Key_0:      CallKey(40, pressed); break;
		case Keys::Key_KP9:
		case Keys::Key_9:      CallKey(41, pressed); break;
		case Keys::Key_KP8:
		case Keys::Key_8:      CallKey(42, pressed); break;
		case Keys::Key_KP7:
		case Keys::Key_7:      CallKey(43, pressed); break;
		case Keys::Key_KP6:
		case Keys::Key_6:      CallKey(44, pressed); break;
		case Keys::Key_P:      CallKey(50, pressed); break;
		case Keys::Key_O:      CallKey(51, pressed); break;
		case Keys::Key_I:      CallKey(52, pressed); break;
		case Keys::Key_U:      CallKey(53, pressed); break;
		case Keys::Key_Y:      CallKey(54, pressed); break;
		case Keys::Key_KP_ENTER:
		case Keys::Key_ENTER:  CallKey(60, pressed); break;
		case Keys::Key_L:      CallKey(61, pressed); break;
		case Keys::Key_K:      CallKey(62, pressed); break;
		case Keys::Key_J:      CallKey(63, pressed); break;
		case Keys::Key_H:      CallKey(64, pressed); break;
		case Keys::Key_SPACE:  CallKey(70, pressed); break;
		case Keys::Key_LALT:   CallKey(71, pressed); break;
		case Keys::Key_M:      CallKey(72, pressed); break;
		case Keys::Key_N:      CallKey(73, pressed); break;
		case Keys::Key_B:      CallKey(74, pressed); break;

		case Keys::Key_COMMA:
			CallKey(71, pressed);
			CallKey(73, pressed);
			break;

		case Keys::Key_QUOTE:
			CallKey(71, pressed);
			CallKey(43, pressed);
			break;

		case Keys::Key_KP_MINUS:
		case Keys::Key_MINUS:
			CallKey(71, pressed);
			CallKey(63, pressed);
			break;

		case Keys::Key_KP_PLUS:
			CallKey(71, pressed);
			CallKey(62, pressed);
			break;

		case Keys::Key_KP_DIVIDE:
			CallKey(71, pressed);
			CallKey(04, pressed);
			break;

		case Keys::Key_KP_MULTIPLY:
			CallKey(71, pressed);
			CallKey(74, pressed);
			break;

		case Keys::Key_KP_PERIOD:
		case Keys::Key_PERIOD:
			CallKey(71, pressed);
			CallKey(72, pressed);
			break;

		case Keys::Key_BACKSPACE:
			CallKey(00, pressed);
			CallKey(40, pressed);
			break;

		case Keys::Key_F5:
			CallKey(00, pressed);
			CallKey(30, pressed);
			break;

		default: break;
	}
}

void Spectrum::DoRemoteKey(RemoteButtons::Enum button, bool pressed) {
	switch (button) {
		case RemoteButtons::Up:
			CallKey(00, pressed);
			CallKey(43, pressed);
			break;
		case RemoteButtons::Down:
			CallKey(00, pressed);
			CallKey(44, pressed);
			break;
		case RemoteButtons::Left:
			CallKey(00, pressed);
			CallKey(34, pressed);
			break;
		case RemoteButtons::Right:
			CallKey(00, pressed);
			CallKey(42, pressed);
			break;
		default:
			break;
	}
}

bool Spectrum::OnKeyPress(Keys::Enum key) {
	DoKey(key, true);
	return true;
}

bool Spectrum::OnKeyUp(Keys::Enum key) {
	DoKey(key, false);
	return true;
}

uint8_t Spectrum::GetPad() const {
	uint32_t buttons = Form::GetButtonsPad1();

	uint8_t pad = 0x00;

	if (buttons & RemoteButtons::SNES_SELECT)
		return 0x00;

	if (buttons & RemoteButtons::Up)
		pad |= 0x08;

	if (buttons & RemoteButtons::Down)
		pad |= 0x04;

	if (buttons & RemoteButtons::Left)
		pad |= 0x02;

	if (buttons & RemoteButtons::Right)
		pad |= 0x01;

	if (buttons & (RemoteButtons::SNES_Y | RemoteButtons::SNES_B))
		pad |= 0x10;

	return pad;
}

bool Spectrum::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	bool ret = false;

	if (m_fileSlot > 0 && (Form::GetButtonsPad1() == RemoteButtons::SPECIAL_SLOT_DECREASE)) {
		m_fileSlot--;
		Console::Write("Estado: ");
		Console::WriteLine(Convert::ToString(m_fileSlot));
	}

	if (Form::GetButtonsPad1() == RemoteButtons::SPECIAL_SLOT_INCREASE) {
		m_fileSlot++;
		Console::Write("Estado: ");
		Console::WriteLine(Convert::ToString(m_fileSlot));
	}

	if (Form::GetButtonsPad1() == RemoteButtons::SPECIAL_LOAD)
		LoadState();

	if (Form::GetButtonsPad1() == RemoteButtons::SPECIAL_SAVE)
		SaveState();

	if (ret)
		return ret;

	DoRemoteKey(button, true);
	uint8_t pad1 = GetPad();
	m_motherboard->OnPadEvent(pad1);

	return true;
}

bool Spectrum::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	if (ArcadeContainer::OnRemoteKeyUp(which, button))
		return true;

	uint8_t pad1 = GetPad();

	bool paused = ((pad1 & 0x40) == 0);
	if (!paused)
		m_pause = false;

	DoRemoteKey(button, false);
	m_motherboard->OnPadEvent(pad1);

	return true;
}

void Spectrum::SetSoundEnabled(bool mode) {
	SoundSDL::Instance()->SetPlayingSound(mode ? m_motherboard->GetSound() : 0);
}

void Spectrum::LoadState() {
	String name = "file.state";
	if (m_fileSlot > 0)
		name = String::Concat(name, Convert::ToString(m_fileSlot));

	if (File::Exists(name)) {
		Console::Write("Cargando: ");
		Console::WriteLine(name);

		uint8_t * savedData = (uint8_t *) calloc (Motherboard::GetSaveSize(), sizeof(uint8_t));
		FileStream * file = new FileStream(name, FileMode::Open, FileAccess::Read);
		for (unsigned int i = 0; i < file->GetLength(); i++)
			savedData[i] = file->ReadByte();
		file->Close();
		delete file;

		m_motherboard->LoadState(savedData);
		free(savedData);
	}
}

void Spectrum::SaveState() {
	String name = "file.state";
	if (m_fileSlot > 0)
		name = String::Concat(name, Convert::ToString(m_fileSlot));

	Console::Write("Guardando: ");
	Console::WriteLine(name);

	uint8_t * savedData = (uint8_t *) calloc (Motherboard::GetSaveSize(), sizeof(uint8_t));
	FileStream * file = new FileStream(name, FileMode::Truncate, FileAccess::Write);
	m_motherboard->SaveState(savedData);

	for (int i = 0; i < Motherboard::GetSaveSize(); i++)
		file->WriteByte(savedData[i]);

	file->Close();
	delete file;

	free(savedData);
}
