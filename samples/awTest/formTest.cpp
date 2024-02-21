// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "formTest.h"

#include "test2.h"
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/Button.h>
#include <awui/Windows/Forms/ControlCollection.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

FormTest::FormTest() {
	this->buttonPressed = RemoteButtons::None;
	this->buttonL = NULL;
	this->InitializeComponent();
}

FormTest::~FormTest() {
}

bool FormTest::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	buttonPressed = button;

	if (buttonPressed == RemoteButtons::Enum::Ok)
		buttonL->SetForeColor(Color::FromArgb(rand.Next(3) * 128 - 1, rand.Next(3) * 128 - 1, rand.Next(3) * 128 - 1));

	return 0;
}

bool FormTest::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	buttonPressed = RemoteButtons::Enum::None;
	return 0;
}

void FormTest::InitializeComponent() {
	this->SetText("Test FormTest");
	this->SetBackColor(Color::FromArgb(255, 0, 0, 0));

	Test2 *test2 = new Test2();
	this->GetControls()->Add(test2);

	Font font = Font("sans-serif", 34, FontStyle::Bold);
	buttonL = new Button();
	buttonL->SetDock(DockStyle::None);
	buttonL->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	buttonL->SetLocation(500, 50);
	buttonL->SetSize(Size(200, 48));
	buttonL->SetText("SILVIA");
	buttonL->SetFont(font);
	this->GetControls()->Add(buttonL);

	this->SetSize(460, 460);
	this->SetFullscreen(0);
}

void FormTest::OnTick(float deltaTime) {
	switch (buttonPressed) {
		case RemoteButtons::Enum::Left:
			buttonL->SetLeft(buttonL->GetLeft() - 5);
			break;
		case RemoteButtons::Enum::Up:
			buttonL->SetTop(buttonL->GetTop() - 5);
			break;
		case RemoteButtons::Enum::Right:
			buttonL->SetLeft(buttonL->GetLeft() + 5);
			break;
		case RemoteButtons::Enum::Down:
			buttonL->SetTop(buttonL->GetTop() + 5);
			break;
		default:
			break;
	}

	Form::OnTick(deltaTime);
}
