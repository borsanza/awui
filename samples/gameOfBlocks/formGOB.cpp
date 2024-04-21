/**
 * samples/gameOfBlocks/FormGOB.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */

#include "formGOB.h"

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

FormGOB::FormGOB() {
	m_renderer = NULL;
	InitializeComponent();
}

FormGOB::~FormGOB() {
	delete m_renderer;
}

void FormGOB::InitializeComponent() {
	SetBackColor(Color::FromArgb(0, 0, 0));

	m_renderer = new Renderer();
	m_renderer->SetDock(DockStyle::Fill);
	// m_renderer->SetSize(500, 500);
	// m_renderer->SetLocation(200, 200);
	AddWidget(m_renderer);

	SetSize(1280, 720);
	SetFullscreen(0);
	SetText("Game Of Blocks");
}

bool FormGOB::OnKeyPress(Keys::Enum key) {
	bool ret = false;
	switch (key) {
		case Keys::Key_5:
			SetSwapInterval(!GetSwapInterval());
			ret = true;
			break;
	}

	return ret;
}
