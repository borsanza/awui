// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include "test1.h"
#include "test2.h"
#include <awui/Drawing/Color.h>
#include <awui/Windows/Forms/Bitmap.h>
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

	Bitmap * bitmap;

	bitmap = new Bitmap("images/button.png");
	bitmap->SetDock(DockStyle::None);
	bitmap->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	bitmap->SetLocation(46, 31);
	bitmap->SetSize(Size(97, 97));
	bitmap->SetFixedMargins(22, 25, 22, 24);
	this->GetControls()->Add(bitmap);

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

	Test2 *test2 = new Test2();
	this->GetControls()->Add(test2);
/*
	Test1 *test1 = new Test1();
	this->GetControls()->Add(test1);
*/

	this->SetSize(400, 472);
	this->SetFullscreen(0);
}

void Form1::OnMouseMove(MouseEventArgs* e) {
	this->bitmap2->SetSize(e->GetX() - 10, e->GetY() - 10);
}
