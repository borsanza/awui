// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include <awui/Windows/Forms/Application.h>

using namespace awui::Windows::Forms;

//#include <awui/Console.h>
//#include <awui/Convert.h>
//#include <awui/Random.h>
//#include <awui/TimeSpan.h>

int main(int argc, char ** argv) {
//	awui::TimeSpan a(10,20,30,40,50);
//	awui::TimeSpan a(1);

//	Console::GetOut()->WriteLine(&a);
/*
	Random a;
	int f = 2;
	for (int i=0; i<=20; i++) {
		f = a.Next(1, 100);
		Console::GetOut()->WriteLine(Convert::ToString(f));
	}
*/
	Form1 *form = new Form1();

	Application::Run(form);

	return 0;
}
