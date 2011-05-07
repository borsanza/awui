// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiApplication.h"
#include "awuiForm.h"

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

struct _ag * awuiApplication::g = 0;

void awuiApplication::Run(awuiForm * form = 0) {
	awuiApplication::g = agNew("prueba");
	ac * c = acNew(awuiApplication::g, 0);
	aw * w = awNew(awuiApplication::g);

	awSetInterval(w, 1);

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
	awDel(w);
	agDel(awuiApplication::g);
}