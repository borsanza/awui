// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include <awui/Windows/Forms/Application.h>

#include <awui/Collections/SortedList.h>
#include <awui/String.h>
#include <awui/Console.h>

using namespace awui::Windows::Forms;

int main(int argc, char ** argv) {
	awui::Collections::SortedList sort;
	sort.Add(new awui::String("b"), new awui::String("je"));
	sort.Add(new awui::String("d"), new awui::String("jo"));
	sort.Add(new awui::String("p"), new awui::String("ju"));
	sort.Add(new awui::String("a"), new awui::String("ja"));
	sort.Add(new awui::String("z"), new awui::String("tururu"));
	sort.Add(new awui::String("c"), new awui::String("ji"));

	for (int i = 0; i < sort.GetCount(); i++)  {
		String * key = (String *) sort.GetKey(i);
		String * value = (String *) sort.GetByIndex(i);
		Console::GetOut()->WriteLine(*key + " - " + *value);
	}

	Form1 *form = new Form1();

	Application::Run(form);

	return 0;
}
