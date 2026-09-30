/**
 * awui/UI/Emulators/Spectrum.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Spectrum.h"

#include <awui/Console.h>
#include <awui/Localization.h>
#include <awui/Convert.h>
#include <awui/Time/ChronoLap.h>
#include <awui/Drawing/Image.h>
#include <awui/Emulation/Common/SavePaths.h>
#include <awui/Emulation/Spectrum/Motherboard.h>
#include <awui/Emulation/Common/AudioOutput.h>
#include <awui/Emulation/Spectrum/TapeCorder.h>
#include <awui/Emulation/Spectrum/ULA.h>
#include <awui/IO/File.h>
#include <awui/OpenGL/GL.h>
#include <awui/UI/Form.h>

#include <set>
#include <vector>

using namespace awui::Drawing;
using namespace awui::Emulation::Spectrum;
using namespace awui::IO;
using namespace awui::OpenGL;
using namespace awui::UI::Emulators;
using namespace awui::Emulation::Common;
using namespace awui::UI;
using namespace awui::UI::Input;

void WriteCassetteCB(int32_t value, void *data) { /* printf("%d\n", value); */
}

int32_t ReadCassetteCB(void *data) {
	TapeCorder *tape = ((Spectrum *) data)->GetTapeCorder();
	if (!tape)
		return 0;

	return tape->GetNext();
}

void FinishCassetteCB(void *data) {
	Spectrum *spectrum = ((Spectrum *) data);
	if (spectrum && spectrum->GetMotherboard())
		spectrum->GetMotherboard()->SetFast(false);
}

Spectrum::Spectrum() {
	SetSize(1, 1);
	m_motherboard = new Motherboard();
	m_motherboard->SetWriteCassetteCB(WriteCassetteCB, this);
	m_motherboard->SetReadCassetteCB(ReadCassetteCB, this);

	m_pause = false;

	m_first = -1;
	m_last = -1;
	m_seconds = 0.0;
	m_heldRemote = 0;
	m_fileSlot = 0;
	m_resetHeld = false;
	m_tapecorder = new TapeCorder();
	m_tapecorder->SetFinishCassetteCB(FinishCassetteCB, this);
	m_motherboard->SetTapeCorder(m_tapecorder);
}

Spectrum::~Spectrum() {
	delete m_motherboard;
}

void Spectrum::LoadRom(const String file) {
	m_romFile = file;
	String ext = file.ToLower();
	if (ext.EndsWith(".rom"))
		m_motherboard->LoadRom(file);

	if (ext.EndsWith(".tap")) {
		String rom = "roms/zxspectrum/48.rom";
		std::vector<String> list = file.Split("/");
		int found = -1;
		String system;
		for (int i = 0; i < (int) list.size(); i++) {
			if (list[i].CompareTo("roms") == 0)
				found = i + 2;

			if (found == i)
				system = String::Concat(list[i], ".rom");
		}

		if (found != -1)
			rom = String::Concat("roms/zxspectrum/", system);

		m_motherboard->LoadRom(rom);
		m_tapecorder->LoadFile(file);
	}

	m_first = 0;
	m_last = 0;
}

void Spectrum::CheckLimits() {
}

void Spectrum::OnTick(float deltaSeconds) {
	// Mientras se mantiene la combinación se sigue reiniciando; el aviso sale una vez
	bool reset = (Form::GetButtonsPad1() == RemoteButtons::SPECIAL_RESET);
	if (reset) {
		m_motherboard->Reset();
		if (!m_resetHeld)
			ShowNotification(Localization::Tr("osd.reset"));
	}
	m_resetHeld = reset;

	// Modo rápido (F8): se emula todo lo que dé tiempo en este tick. Con el cargador de la ROM la carga es
	// instantánea (Motherboard::FlashLoad); con un cargador propio la cinta pasa a toda velocidad
	if (m_motherboard->GetFast()) {
		Time::ChronoLap chrono;
		chrono.Start();
		do {
			m_motherboard->OnTick();
		} while (m_motherboard->GetFast() && (chrono.GetTotalDuration() < 0.030f));

		m_seconds = 0.0;
		return;
	}

	// Se emulan los frames que correspondan al tiempo real (no uno por tick: a 144Hz o sin vsync iría más rápido)
	m_seconds += deltaSeconds;
	if (m_seconds > 0.25) {
		m_seconds = Motherboard::FrameSeconds; // Tras un parón no se intenta recuperar
	}

	while (m_seconds >= Motherboard::FrameSeconds) {
		m_seconds -= Motherboard::FrameSeconds;
		m_motherboard->OnTick();
	}
}

Motherboard *Spectrum::GetCPU() {
	return m_motherboard;
}

// Interface:
// 256x192: 1800 = 6144 bytes = (32 x 8) x 192 bits
// 300 = 768 bytes = 32 x 24 x 8 bits
// 4000-57FF Spectrum bitmap
// 5800-5AFF Spectrum attributes
// 7000 attribute lookup: 256 bytes.  64 colors of (paper, ink)
// 7100 pixel stretch, 16 bytes.

void Spectrum::OnPaint(GL *gl) {
	ULA *ula = m_motherboard->GetULA();

	int width = ula->GetImage()->GetWidth();
	int height = ula->GetImage()->GetHeight();

	// GL::DrawImageGL(ula->GetImage(), 0, 0, GetWidth(), GetHeight());

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

// Teclas de la matriz del Spectrum (fila * 10 + columna) que pulsa cada tecla del PC
static void GetMatrixKeys(Keys::Enum key, std::vector<int> &keys) {
	switch (key) {
		case Keys::Key_LSHIFT:
		case Keys::Key_RSHIFT:
			keys.push_back(0);
			break;
		case Keys::Key_Z:
			keys.push_back(1);
			break;
		case Keys::Key_X:
			keys.push_back(2);
			break;
		case Keys::Key_C:
			keys.push_back(3);
			break;
		case Keys::Key_V:
			keys.push_back(4);
			break;
		case Keys::Key_A:
			keys.push_back(10);
			break;
		case Keys::Key_S:
			keys.push_back(11);
			break;
		case Keys::Key_D:
			keys.push_back(12);
			break;
		case Keys::Key_F:
			keys.push_back(13);
			break;
		case Keys::Key_G:
			keys.push_back(14);
			break;
		case Keys::Key_Q:
			keys.push_back(20);
			break;
		case Keys::Key_W:
			keys.push_back(21);
			break;
		case Keys::Key_E:
			keys.push_back(22);
			break;
		case Keys::Key_R:
			keys.push_back(23);
			break;
		case Keys::Key_T:
			keys.push_back(24);
			break;
		case Keys::Key_KP1:
		case Keys::Key_1:
			keys.push_back(30);
			break;
		case Keys::Key_KP2:
		case Keys::Key_2:
			keys.push_back(31);
			break;
		case Keys::Key_KP3:
		case Keys::Key_3:
			keys.push_back(32);
			break;
		case Keys::Key_KP4:
		case Keys::Key_4:
			keys.push_back(33);
			break;
		case Keys::Key_KP5:
		case Keys::Key_5:
			keys.push_back(34);
			break;
		case Keys::Key_KP0:
		case Keys::Key_0:
			keys.push_back(40);
			break;
		case Keys::Key_KP9:
		case Keys::Key_9:
			keys.push_back(41);
			break;
		case Keys::Key_KP8:
		case Keys::Key_8:
			keys.push_back(42);
			break;
		case Keys::Key_KP7:
		case Keys::Key_7:
			keys.push_back(43);
			break;
		case Keys::Key_KP6:
		case Keys::Key_6:
			keys.push_back(44);
			break;
		case Keys::Key_P:
			keys.push_back(50);
			break;
		case Keys::Key_O:
			keys.push_back(51);
			break;
		case Keys::Key_I:
			keys.push_back(52);
			break;
		case Keys::Key_U:
			keys.push_back(53);
			break;
		case Keys::Key_Y:
			keys.push_back(54);
			break;
		case Keys::Key_KP_ENTER:
		case Keys::Key_ENTER:
			keys.push_back(60);
			break;
		case Keys::Key_L:
			keys.push_back(61);
			break;
		case Keys::Key_K:
			keys.push_back(62);
			break;
		case Keys::Key_J:
			keys.push_back(63);
			break;
		case Keys::Key_H:
			keys.push_back(64);
			break;
		case Keys::Key_SPACE:
			keys.push_back(70);
			break;
		case Keys::Key_LALT:
		case Keys::Key_RALT:
		case Keys::Key_LCTRL:
		case Keys::Key_RCTRL:
			keys.push_back(71);
			break;
		case Keys::Key_M:
			keys.push_back(72);
			break;
		case Keys::Key_N:
			keys.push_back(73);
			break;
		case Keys::Key_B:
			keys.push_back(74);
			break;

		case Keys::Key_COMMA:
			keys.push_back(71);
			keys.push_back(73);
			break;

		case Keys::Key_QUOTE:
			keys.push_back(71);
			keys.push_back(43);
			break;

		case Keys::Key_KP_MINUS:
		case Keys::Key_MINUS:
			keys.push_back(71);
			keys.push_back(63);
			break;

		case Keys::Key_KP_PLUS:
		case Keys::Key_PLUS:
			keys.push_back(71);
			keys.push_back(62);
			break;

		case Keys::Key_KP_DIVIDE:
			keys.push_back(71);
			keys.push_back(4);
			break;

		case Keys::Key_KP_MULTIPLY:
			keys.push_back(71);
			keys.push_back(74);
			break;

		case Keys::Key_KP_PERIOD:
		case Keys::Key_PERIOD:
			keys.push_back(71);
			keys.push_back(72);
			break;

		case Keys::Key_LESS:
			keys.push_back(71);
			keys.push_back(23);
			break;

		case Keys::Key_BACKSPACE:
			keys.push_back(0);
			keys.push_back(40);
			break;

		case Keys::Key_F5:
			keys.push_back(0);
			keys.push_back(30);
			break;

		default:
			break;
	}
}

void Spectrum::DoKey(Keys::Enum key, bool pressed) {
	switch (key) {
		case Keys::Key_F2:
			if (pressed)
				SaveState();
			break;
		case Keys::Key_F4:
			if (pressed)
				LoadState();
			break;
		// La cinta la pone en marcha y la para el propio cargador (Motherboard::UpdateTapeMotor), como el motor de
		// un casete con control remoto: estas teclas no la hacen sonar
		case Keys::Key_F8:
			// Carga ultrarrápida: con el cargador de la ROM es instantánea; con uno propio la cinta pasa a toda
			// velocidad
			if (pressed) {
				m_motherboard->SetFast(!m_motherboard->GetFast());
				ShowNotification(Localization::Tr(m_motherboard->GetFast() ? "osd.fastOn" : "osd.fastOff"));
			}
			break;
		case Keys::Key_F9:
			// Rebobina la cinta al principio (para volver a cargar); arranca sola al hacer LOAD ""
			if (pressed) {
				m_tapecorder->Stop();
				m_tapecorder->Rewind();
				ShowNotification(Localization::Tr("osd.tapeRewound"));
			}
			break;
		default:
			break;
	}

	std::vector<int> keys;
	GetMatrixKeys(key, keys);
	if (keys.empty())
		return;

	// Se apunta qué teclas del PC están pulsadas y se recalcula la matriz entera: así una tecla del Spectrum
	// compartida (Caps o Symbol Shift) sigue pulsada mientras la mantenga cualquier tecla del PC
	if (pressed)
		m_heldKeys.insert(key);
	else
		m_heldKeys.erase(key);

	UpdateMatrix();
}

// Las flechas son las teclas de cursor del Spectrum (Caps Shift + 5/6/7/8), además del joystick Kempston
void Spectrum::DoRemoteKey(RemoteButtons::Enum button, bool pressed) {
	switch (button) {
		case RemoteButtons::Up:
		case RemoteButtons::Down:
		case RemoteButtons::Left:
		case RemoteButtons::Right:
			if (pressed)
				m_heldRemote |= button;
			else
				m_heldRemote &= ~button;

			UpdateMatrix();
			break;
		default:
			break;
	}
}

// Recalcula la matriz del teclado del Spectrum a partir de todo lo que está pulsado
void Spectrum::UpdateMatrix() {
	std::set<int> matrix;
	bool shift = false;
	bool symbolKey = false;

	for (Keys::Enum key : m_heldKeys) {
		if ((key == Keys::Key_LSHIFT) || (key == Keys::Key_RSHIFT)) {
			shift = true;
			continue;
		}

		std::vector<int> keys;
		GetMatrixKeys(key, keys);
		// Un signo que es Symbol Shift + tecla (coma, punto, +...)
		if ((keys.size() == 2) && (keys[0] == 71))
			symbolKey = true;

		matrix.insert(keys.begin(), keys.end());
	}

	// Mayúsculas + un signo sería Caps + Symbol Shift (modo extendido): se escribe solo el signo
	if (shift && !symbolKey)
		matrix.insert(0);

	static const struct {
		uint32_t button;
		int key;
	} cursors[] = {{RemoteButtons::Up, 43}, {RemoteButtons::Down, 44}, {RemoteButtons::Left, 34}, {RemoteButtons::Right, 42}};
	for (const auto &cursor : cursors) {
		if (m_heldRemote & cursor.button) {
			matrix.insert(0);
			matrix.insert(cursor.key);
		}
	}

	m_motherboard->ReleaseAllKeys();
	for (int key : matrix)
		m_motherboard->OnKeyPress(key / 10, 1 << (key % 10));
}

void Spectrum::ReleaseAllKeys() {
	m_heldKeys.clear();
	m_heldRemote = 0;
	UpdateMatrix();
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
		ShowNotification(String(Localization::Tr("osd.slot").ToCharArray(), m_fileSlot));
	}

	if (Form::GetButtonsPad1() == RemoteButtons::SPECIAL_SLOT_INCREASE) {
		m_fileSlot++;
		ShowNotification(String(Localization::Tr("osd.slot").ToCharArray(), m_fileSlot));
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
	// Deja de ser el juego activo (se sale al menú): lo que estuviera pulsado ya no recibirá el soltar
	if (!mode)
		ReleaseAllKeys();

	AudioOutput::Instance().SetPlaying(mode ? m_motherboard->GetSound() : 0);
}

// Estado junto a la cinta o la ROM del juego: DynamiteDan.tap.state, y con ranura DynamiteDan.tap.state1...
awui::String Spectrum::GetStateFile() const {
	String name = String::Concat(m_romFile, ".state");
	if (m_fileSlot > 0)
		name = String::Concat(name, Convert::ToString(m_fileSlot));

	return name;
}

bool Spectrum::SaveAutoState() {
	std::vector<uint8_t> data(Motherboard::GetSaveSize());
	m_motherboard->SaveState(data.data());
	return WriteStateFile(SavePaths::GetWritePath(String::Concat(m_romFile, ".autostate")), data.data(), (int) data.size());
}

bool Spectrum::LoadAutoState() {
	std::vector<uint8_t> data(Motherboard::GetSaveSize());
	if (!ReadStateFile(SavePaths::GetReadPath(String::Concat(m_romFile, ".autostate")), data.data(), (int) data.size()))
		return false;

	m_motherboard->LoadState(data.data());
	ReleaseAllKeys();
	return true;
}

void Spectrum::LoadState() {
	String name = SavePaths::GetReadPath(GetStateFile());

	if (!File::Exists(name)) {
		ShowNotification(String(Localization::Tr("osd.stateMissing").ToCharArray(), m_fileSlot));
		return;
	}

	Console::WriteLine(String("Cargando: ") + name);

	// Un estado de otro tamaño es de otra versión del emulador: ReadStateFile no lo carga (desbordaría el buffer o
	// dejaría la máquina a medias) y lo avisa
	std::vector<uint8_t> savedData(Motherboard::GetSaveSize());
	if (ReadStateFile(name, savedData.data(), (int) savedData.size())) {
		m_motherboard->LoadState(savedData.data());
		ShowNotification(String(Localization::Tr("osd.stateLoaded").ToCharArray(), m_fileSlot));
	}
}

void Spectrum::SaveState() {
	String name = SavePaths::GetWritePath(GetStateFile());

	Console::WriteLine(String("Guardando: ") + name);

	std::vector<uint8_t> savedData(Motherboard::GetSaveSize());
	m_motherboard->SaveState(savedData.data());
	if (WriteStateFile(name, savedData.data(), (int) savedData.size()))
		ShowNotification(String(Localization::Tr("osd.stateSaved").ToCharArray(), m_fileSlot));
}
