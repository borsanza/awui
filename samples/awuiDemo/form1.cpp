// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include "test1.h"
#include <awui/Drawing/Color.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Form1::Form1() {
	m_bitmap2 = nullptr;
	InitializeComponent();
}

void Form1::InitializeComponent() {
	SetText("Test Form1");
	SetBackColor(Color::FromArgb(255, 0, 0, 0));

	Test1 *test1 = new Test1();
	AddWidget(test1);

	SetSize(460, 532);
	SetFullscreen(0);
}
