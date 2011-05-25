// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <stdarg.h>

#include "awuiForm.h"

#include "awuiApplication.h"
#include "awuiArrayList.h"
#include "awuiBitmap.h"
#include "awuiGraphics.h"

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

#ifndef GL_BGRA
	#define GL_BGRA 0x80E1
#endif

awuiForm::awuiForm() {
	this->x = 100;
	this->y = 100;
	this->width = 300;
	this->height = 300;
	this->mouseButtons = 0;

	glGenTextures(0, &this->texture1);
	glGenTextures(1, &this->texture2);

	this->old1w = -1;
	this->old1h = -1;
	this->old2w = -1;
	this->old2h = -1;
}

awuiForm::~awuiForm() {
	glDeleteTextures(1, &this->texture2);
	glDeleteTextures(0, &this->texture1);

	awMakeCurrent(this->w, NULL);
	awDel(this->w);
}

int awuiForm::IsClass(awuiObject::awuiClasses objectClass) {
	if (objectClass == awuiObject::Form)
		return 1;

	return awuiControl::IsClass(objectClass);
}

void awuiForm::Show() {
	this->w = awNew(awuiApplication::g);
	awGeometry(this->w, this->x, this->y, this->width, this->height);
	awShow(this->w);
}

void awuiForm::OnPaintForm() {
	glViewport(0, 0, this->GetWidth(), this->GetHeight());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, this->GetWidth(), this->GetHeight(), 0.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_SCISSOR_TEST);
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_SCISSOR_TEST);

	this->OnPaintPre(0, 0, this->GetWidth(), this->GetHeight());
}

void awuiForm::ProcessEvents(ac * c) {
	const ae * e;
	aw * w = this->w;

	int resizex = -1;
	int resizey = -1;
	
	while ((e = awNextEvent(w))) {
		switch (aeType(e)) {
			case AW_EVENT_RESIZE:
				resizex = aeWidth(e);
				resizey = aeHeight(e);
				break;
			case AW_EVENT_DOWN: {
					MouseButtons::Buttons button = MouseButtons::None;
					switch (aeWhich(e)) {
						case AW_KEY_MOUSEWHEELUP:
							button = MouseButtons::XButton1;
							break;
						case AW_KEY_MOUSEWHEELDOWN:
							button = MouseButtons::XButton2;
							break;
						case AW_KEY_MOUSELEFT:
							button = MouseButtons::Left;
							break;
						case AW_KEY_MOUSERIGHT:
							button = MouseButtons::Right;
							break;
						case AW_KEY_MOUSEMIDDLE:
							button = MouseButtons::Middle;
							break;
					}

					if (button) {
						this->mouseButtons |= button;
						this->OnMouseDownPre(button, this->mouseButtons);
					}
				}
				break;
			case AW_EVENT_UP: {
					MouseButtons::Buttons button = MouseButtons::None;
					switch (aeWhich(e)) {
						case AW_KEY_MOUSEWHEELUP:
							button = MouseButtons::XButton1;
							break;
						case AW_KEY_MOUSEWHEELDOWN:
							button = MouseButtons::XButton2;
							break;
						case AW_KEY_MOUSELEFT:
							button = MouseButtons::Left;
							break;
						case AW_KEY_MOUSERIGHT:
							button = MouseButtons::Right;
							break;
						case AW_KEY_MOUSEMIDDLE:
							button = MouseButtons::Middle;
							break;
					}

					if (button) {
						this->mouseButtons &= ~button;
						this->OnMouseUpPre(button, this->mouseButtons);
					}
				}
				break;
/*
			case AW_EVENT_CLOSE:
				g_exit = 1;
				break;
			case AW_EVENT_UNICODE:
				Log("Unicode: %s", awKeyName(awe->u.unicode.which));
				break;
*/
			case AW_EVENT_MOTION:
				this->OnMouseMovePre(aeX(e), aeY(e), this->mouseButtons);
				break;
			default:
				break;
		}
	}

	if ((resizex != -1) && (resizey != -1)) {
		this->width = resizex;
		this->height = resizey;
		this->OnResizePre();
	}
}