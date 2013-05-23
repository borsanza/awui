// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include "test1.h"
#include "test2.h"
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/Button.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/MouseEventArgs.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Form1::Form1() {
	this->InitializeComponent();
}

Form1::~Form1() {
}

void Form1::InitializeComponent() {
	this->SetText("Test Form1");
	this->SetBackColor(Color::FromArgb(255, 0, 0, 0));

/*
	bitmap2 = new Bitmap("images/button.png");
	bitmap2->SetDock(DockStyle::None);
	bitmap2->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	bitmap2->SetLocation(0, 0);
	bitmap2->SetSize(Size(400, 400));
	bitmap2->SetFixedMargins(22, 25, 22, 24);
//	bitmap2->SetFixedMargins(26, 20, 26, 22); // Button 5
//	bitmap2->SetFixedMargins(9, 9, 9, 9); // Button 6
//	bitmap2->SetStretchMode(StretchMode::Tile);
	this->GetControls()->Add(bitmap2);
*/

	Test2 *test2 = new Test2();
	this->GetControls()->Add(test2);
/*
	test2 = new Test2();
	this->GetControls()->Add(test2);
*/

	Test1 *test1 = new Test1();
	this->GetControls()->Add(test1);

	Font font = Font("sans-serif", 34, FontStyle::Bold);
	Button * button = new Button();
	button->SetDock(DockStyle::None);
	button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	button->SetLocation(100, 1000);
	button->SetSize(Size(600, 100));
	button->SetText("Larguisimo");
	button->SetFont(&font);
	this->GetControls()->Add(button);
	button = new Button();
	button->SetDock(DockStyle::None);
	button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	button->SetLocation(400, 900);
	button->SetSize(Size(100, 100));
	button->SetText("No");
	button->SetFont(&font);
	this->GetControls()->Add(button);


	this->SetSize(460, 532);
	this->SetFullscreen(0);
}

/*
void Form1::OnMouseMove(MouseEventArgs* e) {
	this->bitmap2->SetSize(e->GetX() - 10, e->GetY() - 10);
}
*/
