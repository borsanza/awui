// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include "test1.h"
#include <awui/Drawing/Color.h>
#include <awui/Windows/Forms/ControlCollection.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Form1::Form1() {
	this->bitmap2 = NULL;
	this->InitializeComponent();
}

Form1::~Form1() {
}

void Form1::InitializeComponent() {
	this->SetText("Test Form1");
	this->SetBackColor(Color::FromArgb(255, 0, 0, 0));

	Test1 *test1 = new Test1();
	this->GetControls()->Add(test1);

	this->SetSize(460, 532);
	this->SetFullscreen(0);
}
