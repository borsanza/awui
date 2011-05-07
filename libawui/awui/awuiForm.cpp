// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiForm.h"

#include "awuiBitmap.h"
#include "awuiGraphics.h"
#include "awuiColor.h"
#include "awuiApplication.h"

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

#include <cairo.h>

awuiForm::awuiForm() {
	this->x = 10;
	this->y = 10;
	this->width = 300;
	this->height = 300;
	this->backColor = awuiColor::FromArgb(226, 226, 226);
	this->bitmap = 0;
	this->OnResizePre();
}

awuiForm::~awuiForm() {
	awMakeCurrent(this->w, 0);
	awDel(this->w);

	if (this->backColor != 0)
		delete this->backColor;
}

void awuiForm::Show() {
	this->w = awNew(awuiApplication::g);
	awGeometry(this->w, this->x, this->y, this->width, this->height);
	awShow(this->w);
}

#ifndef GL_BGRA
	#define GL_BGRA 0x80E1
#endif //GL_BGRA

void awuiForm::OnResizePre() {
	if (this->bitmap != 0)
		delete this->bitmap;

	this->bitmap = new awuiBitmap(this->GetWidth(), this->GetHeight());
}

void awuiForm::OnPaintPre() {
	glViewport(0, 0, this->GetWidth(), this->GetHeight());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(0.0f, this->GetWidth(),this->GetHeight(), 0.0f,-1.0f,1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);					// Enable Depth Testing


	awuiGraphics * g = awuiGraphics::FromImage(this->bitmap);
	this->OnPaint(g);
	delete g;

	glEnable(GL_TEXTURE_2D);

	GLuint texture;
  glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->GetWidth(), this->GetHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, this->bitmap->image);

	glBegin(GL_QUADS);						// Begin Drawing Quads
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);		// Set The Alpha Value (Starts At 0.2)
	glTexCoord2f(0,1);			// Texture Coordinate	(   0,   1 )
	glVertex2f(0,0);				// First Vertex		(   0,   0 )

	glTexCoord2f(0,0);			// Texture Coordinate	(   0,   0 )
	glVertex2f(0,this->GetHeight());				// Second Vertex	(   0, 480 )

	glTexCoord2f(1,0);			// Texture Coordinate	(   1,   0 )
	glVertex2f(this->GetWidth(),this->GetHeight());				// Third Vertex		( 640, 480 )

	glTexCoord2f(1,1);			// Texture Coordinate	(   1,   1 )
	glVertex2f(this->GetWidth(),0);				// Fourth Vertex	( 640,   0 )
	glEnd();							// Done Drawing Quads

	glDeleteTextures(1, &texture);

	glDisable(GL_TEXTURE_2D);					// Disable 2D Texture Mapping
}

void awuiForm::SetBackColor(awuiColor * color) {
	if (this->backColor != 0)
		delete this->backColor;

	this->backColor = awuiColor::FromArgb(color->ToArgb());
}

awuiColor * awuiForm::GetBackColor() {
	return awuiColor::FromArgb(this->backColor->ToArgb());
}

void awuiForm::ProcessEvents(ac * c) {
	const ae * e;
	aw * w = this->w;

	while ((e = awNextEvent(w))) {
		switch (aeType(e)) {
			case AW_EVENT_RESIZE:
				this->width = aeWidth(e);
				this->height = aeHeight(e);
				this->OnResizePre();
				this->OnResize();
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
			default:
				break;
		}
	}
}

