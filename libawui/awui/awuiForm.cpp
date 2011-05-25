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
	static int pos = 0;
	static int first = 1;
	glViewport(0, 0, this->GetWidth(), this->GetHeight());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	awuiGraphics * g = awuiGraphics::FromImage(this->bitmap);
	this->OnPaintPre(g);
	delete g;

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	glBindTexture(GL_TEXTURE_2D, pos? this->texture2 : this->texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	if (((this->old1w == this->GetWidth()) && (this->old1h == this->GetHeight()) && !pos)  ||
		((this->old2w == this->GetWidth()) && (this->old2h == this->GetHeight()) && pos))
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->GetWidth(), this->GetHeight(), GL_BGRA, GL_UNSIGNED_BYTE, this->bitmap->image);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->GetWidth(), this->GetHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, this->bitmap->image);

	if (pos) {
		this->old2w = this->GetWidth();
		this->old2h = this->GetHeight();
	} else {
		this->old1w = this->GetWidth();
		this->old1h = this->GetHeight();
	}

	if (!first) {
		glBindTexture(GL_TEXTURE_2D, pos? this->texture1 : this->texture2);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glEnd();
	}

	first = 0;
	pos = !pos;
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