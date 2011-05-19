// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <stdarg.h>
#include <stdio.h>

#include "awuiForm.h"

#include "awuiApplication.h"
#include "awuiArrayList.h"
#include "awuiBitmap.h"
#include "awuiGraphics.h"

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

#include <stdlib.h>

#ifndef GL_BGRA
	#define GL_BGRA 0x80E1
#endif

awuiForm::awuiForm() {
	this->x = 10;
	this->y = 10;
	this->width = 300;
	this->height = 300;
}

awuiForm::~awuiForm() {
	awMakeCurrent(this->w, NULL);
	awDel(this->w);
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
//	glOrtho(0.0f, this->GetWidth(), this->GetHeight(), 0.0f, -1.0f, 1.0f);
	glOrtho(0.0f, this->GetWidth(), 0.0f, this->GetHeight(), -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);

	awuiGraphics * g = awuiGraphics::FromImage(this->bitmap);
	this->OnPaintPre(g);
	delete g;

	glEnable(GL_TEXTURE_2D);

	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->GetWidth(), this->GetHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, this->bitmap->image);

	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(0.0f, (float)this->GetHeight());

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f((float)this->GetWidth(), (float)this->GetHeight());

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f((float)this->GetWidth(), 0.0f);
	glEnd();

	glDeleteTextures(1, &texture);

	glDisable(GL_TEXTURE_2D);
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
/*
			case AW_EVENT_CLOSE:
				g_exit = 1;
				break;

/*
			case AW_EVENT_UNICODE:
				Log("Unicode: %s", awKeyName(awe->u.unicode.which));
				break;
			case AW_EVENT_DOWN:
				if (awe->u.down.which == 'f') {
					awClose(w);
					w = awOpenFullscreen();
					awMakeCurrent(w, c);
				}
				if (awe->u.down.which == 'b') {
					awClose(w);
					w = awOpenBorderless(100, 100, 300, 400);
					awMakeCurrent(w, c);
				}
				if (awe->u.down.which == 'w') {
					awClose(w);
					w = awOpen(100, 100, 300, 400);
					awMakeCurrent(w, c);
				}
				if (awe->u.down.which == 'm') {
					awClose(w);
					w = awOpenMaximized();
					awMakeCurrent(w, c);
				}
				if (awe->u.down.which == 'q')
					g_exit = 1;
				Log("Down: %s", awKeyName(awe->u.down.which));
				break;
			case AW_EVENT_UP:
				Log("Up: %s", awKeyName(awe->u.up.which));
				break;
			case AW_EVENT_MOTION:
				Log("Motion: %d,%d", awe->u.motion.x, awe->u.motion.y);
				break;
*/
			case AW_EVENT_MOTION:
				printf("Motion: %dx%d\n", aeX(e), aeY(e));
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