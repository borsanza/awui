// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "formTest.h"

#include "test2.h"
#include <awui/Core/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/Button.h>

using namespace awui;
using namespace awui::Drawing;
using namespace awui::Windows::Forms;

FormTest::FormTest() {
	m_buttonPressed = RemoteButtons::None;
	m_buttonL = NULL;
	InitializeComponent();
}

bool FormTest::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	m_buttonPressed = button;

	if (m_buttonPressed == RemoteButtons::Enum::Ok)
		m_buttonL->SetForeColor(Color::FromArgb(m_rand.Next(3) * 128 - 1, m_rand.Next(3) * 128 - 1, m_rand.Next(3) * 128 - 1));

	return 0;
}

bool FormTest::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	m_buttonPressed = RemoteButtons::Enum::None;
	return 0;
}

void FormTest::InitializeComponent() {
	SetText("Test FormTest");
	SetBackColor(Color::FromArgb(255, 0, 0, 0));

	Test2 *test2 = new Test2();
	AddWidget(test2);

	Font font = Font("sans-serif", 34, FontStyle::Bold);
	m_buttonL = new Button();
	m_buttonL->SetDock(DockStyle::None);
	m_buttonL->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	m_buttonL->SetLocation(500, 50);
	m_buttonL->SetSize(Size(200, 48));
	m_buttonL->SetText("SILVIA");
	m_buttonL->SetFont(font);
	AddWidget(m_buttonL);

	SetSize(460, 460);
	SetFullscreen(0);
}

void FormTest::OnTick(float deltaSeconds) {
	switch (m_buttonPressed) {
		case RemoteButtons::Enum::Left:
			m_buttonL->SetLeft(m_buttonL->GetLeft() - 5);
			break;
		case RemoteButtons::Enum::Up:
			m_buttonL->SetTop(m_buttonL->GetTop() - 5);
			break;
		case RemoteButtons::Enum::Right:
			m_buttonL->SetLeft(m_buttonL->GetLeft() + 5);
			break;
		case RemoteButtons::Enum::Down:
			m_buttonL->SetTop(m_buttonL->GetTop() + 5);
			break;
		default:
			break;
	}

	Form::OnTick(deltaSeconds);
}
