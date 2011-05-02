// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiApplication.h"
#include "awuiForm.h"

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

void awuiApplication::Run(awuiForm * form = 0) {
	if (!awInit())
		return;

	ac * c = acNew(0);

	acSetInterval(c, 1);

	form->Show();

	int g_exit = 0;
	while (!g_exit) {
		awMakeCurrent(form->w, c);
		form->ProcessEvents(c);
		if (!g_exit) {
			form->OnPaintPre();
			glFlush();
			awSwapBuffers(form->w);
		}
	}

	awMakeCurrent(form->w, 0);

	acDel(c);
	awEnd();
}