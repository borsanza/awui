/**
 * samples/gameOfBlocks/FormGOB.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */

#include "formGOB.h"

#include "gameView.h"

#include <awui/Drawing/Font.h>
#include <awui/UI/Label.h>

using namespace awui;
using namespace awui::Drawing;
using namespace awui::UI;
using namespace awui::UI::Input;

namespace {
	const int InfoLines = 7;
	const int InfoLineHeight = 22;
	const int InfoWidth = 640;
	const int InfoMargin = 10;
	const float InfoInterval = 0.1f; // El texto se rehace cada vez: no hace falta en todos los frames
} // namespace

FormGOB::FormGOB() {
	m_view = NULL;
	m_infoSeconds = 0.0f;
	InitializeComponent();
}

FormGOB::~FormGOB() {
}

void FormGOB::InitializeComponent() {
	SetBackColor(Color::Black);

	m_view = new GameView();
	m_view->SetDock(DockStyle::Fill);
	AddWidget(m_view);

	// Fondo negro medio transparente, como el panel de la versión web
	Font font("Liberation Mono", 14);
	for (int i = 0; i < InfoLines; i++) {
		Label *label = new Label();
		label->SetFont(font);
		label->SetForeColor(Color::White);
		label->SetBackColor(Color::FromArgb(128, 0, 0, 0));
		label->SetTextAlign(ContentAlignment::MiddleLeft);
		label->SetSize(InfoWidth, InfoLineHeight);
		AddWidget(label);
		m_info.push_back(label);
	}

	SetSize(1280, 720);
	SetFullscreen(0);
	SetText("Game Of Blocks");
	UpdateInfo();
}

void FormGOB::UpdateInfo() {
	std::vector<String> lines = m_view->GetInfo();
	lines.push_back(m_view->IsFirstPerson() ? (m_view->IsMouseCaptured() ? "WASD Space Ctrl | 5 camera  6 wireframe  7 axes | Esc: release mouse" : "Click to look around | 5 camera  6 wireframe  7 axes")
											: "WASD Space Ctrl | drag to orbit | 5 camera  6 wireframe  7 axes");

	// Encima de la barra de estadísticas (los FPS), que está al pie
	int bottom = GetHeight() - InfoMargin - 24;
	for (int i = 0; i < (int) m_info.size(); i++) {
		String text = (i < (int) lines.size()) ? (String(" ") + lines[i]) : String("");
		if (m_info[i]->GetText().CompareTo(text) != 0)
			m_info[i]->SetText(text);

		m_info[i]->SetLocation(InfoMargin, bottom - ((int) m_info.size() - i) * InfoLineHeight);
	}
}

void FormGOB::OnTick(float deltaSeconds) {
	Form::OnTick(deltaSeconds);

	m_infoSeconds += deltaSeconds;
	if (m_infoSeconds >= InfoInterval) {
		m_infoSeconds = 0.0f;
		UpdateInfo();
	}
}

bool FormGOB::OnKeyPress(Keys::Enum key) {
	if (key == Keys::Key_F10) {
		SetSwapInterval(!GetSwapInterval());
		return true;
	}

	return m_view->KeyDown(key);
}

bool FormGOB::OnKeyUp(Keys::Enum key) {
	return m_view->KeyUp(key);
}

// Escape (el botón de menú) suelta el ratón
bool FormGOB::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	if ((button & RemoteButtons::Menu) && m_view->ReleaseMouse())
		return true;

	return Form::OnRemoteKeyUp(which, button);
}
