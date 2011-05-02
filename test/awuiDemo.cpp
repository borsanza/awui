// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/awuiApplication.h>
#include "form1.h"

int main(int argc, char ** argv) {
	Form1 *form = new Form1();

	awuiApplication::Run(form);

	return 0;
}
