// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include "test1.h"
#include "test2.h"
#include <awui/Drawing/Color.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/ControlCollection.h>

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

	Bitmap * bitmap = new Bitmap("images/button.png");
	bitmap->SetDock(DockStyle::None);
	bitmap->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	bitmap->SetLocation(46, 31);
	bitmap->SetSize(Size(97, 97));
	bitmap->SetFixedMargins(22, 25, 22, 24);
	this->GetControls()->Add(bitmap);

/*
	Bitmap * bitmap = new Bitmap("images/button4.png");
	bitmap->SetDock(DockStyle::None);
	bitmap->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	bitmap->SetLocation(0, 0);
	bitmap->SetSize(Size(400, 400));
	bitmap->SetFixedMargins(22, 25, 22, 24);
	bitmap->SetStretchMode(StretchMode::Tile);
	this->GetControls()->Add(bitmap);
*/
	Test2 *test2 = new Test2();
	this->GetControls()->Add(test2);
/*
	Test1 *test1 = new Test1();
	this->GetControls()->Add(test1);
*/

	this->SetSize(400, 472);
	this->SetFullscreen(0);
}
