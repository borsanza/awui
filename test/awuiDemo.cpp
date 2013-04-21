// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include <awui/Windows/Forms/Application.h>

using namespace awui::Windows::Forms;

//#include <awui/Console.h>
//#include <awui/Random.h>
//#include <awui/Int32.h>

int main(int argc, char ** argv) {
/*
	Random a;
	int f = 2;
	for (int i=0; i<=20; i++) {
		f = a.Next(1, 100);
		Int32 f2(f);
		Console::GetOut()->WriteLine(&f2);
	}
*/
	Form1 *form = new Form1();

	Application::Run(form);

	return 0;
}
