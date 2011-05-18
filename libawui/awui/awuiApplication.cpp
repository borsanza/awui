// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiApplication.h"
#include "awuiForm.h"
#include <unistd.h>

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

struct _ag * awuiApplication::g = NULL;

void awuiApplication::Run(awuiForm * form = NULL) {
	awuiApplication::g = agNew("prueba");
	ac * c = acNew(awuiApplication::g, NULL);
	aw * w = awNew(awuiApplication::g);

	awSetInterval(w, 1);

	form->Show();

	int g_exit = 0;
	while (!g_exit) {
		awMakeCurrent(form->w, c);
		form->ProcessEvents(c);
		if (!g_exit) {
			form->OnPaintForm();
			glFlush();
			awSwapBuffers(form->w);
			usleep(16000);
		}
	}

	awMakeCurrent(form->w, NULL);

	acDel(c);
	awDel(w);
	agDel(awuiApplication::g);
}
