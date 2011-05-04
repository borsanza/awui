// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiForm.h"

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

#include <cairo.h>

#include <cairomm/context.h>
#include <cairomm/surface.h>

awuiForm::awuiForm() {
	this->x = 10;
	this->y = 10;
	this->width = 300;
	this->height = 300;
	this->SetBackColor(226, 226, 226);
}

awuiForm::~awuiForm() {
	awMakeCurrent(this->w, 0);
	awClose(this->w);
}

void awuiForm::Show() {
	this->w = awOpen(this->x, this->y, this->width, this->height);
}

void awuiForm::OnPaintPre() {
	glViewport(0, 0, this->GetWidth(), this->GetHeight());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(0.0f, this->GetWidth(),this->GetHeight(), 0.0f,-1.0f,1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

/*
	glClearColor(this->red / 255.0, this->green / 255.0, this->blue / 255.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
*/

	glDisable(GL_DEPTH_TEST);					// Enable Depth Testing


	static int sube = 1;
	static float py = (float) this->GetHeight();

	if (sube)
		py--;
	else
		py++;

	if (py < 0) {
		py = 0;
		sube = 0;
	}

	if (py > this->GetHeight()) {
		sube = 1;
		py = (float) this->GetHeight();
	}


	GLuint tex;
  glGenTextures(1, &tex);

	Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, this->GetWidth(), this->GetHeight());

	Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

	cr->set_source_rgb(1.0, 0.0, 0.0);
	cr->paint(); // fill image with the color

	cr->set_source_rgb(0.0, 1.0, 0.0);
	cr->move_to (0, py);
	cr->line_to (this->GetWidth(), py);
	cr->stroke();
	cr->move_to (0, this->GetHeight() - py);
	cr->line_to (this->GetWidth(), this->GetHeight() - py);
	cr->stroke();

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->get_width(), surface->get_height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->get_data());


	glBegin(GL_QUADS);						// Begin Drawing Quads
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);		// Set The Alpha Value (Starts At 0.2)
	glTexCoord2f(0,1);			// Texture Coordinate	(   0,   1 )
	glVertex2f(0,0);				// First Vertex		(   0,   0 )

	glTexCoord2f(0,0);			// Texture Coordinate	(   0,   0 )
	glVertex2f(0,this->GetHeight());				// Second Vertex	(   0, 480 )

	glTexCoord2f(1,0);			// Texture Coordinate	(   1,   0 )
	glVertex2f(this->GetWidth(),this->GetHeight());				// Third Vertex		( 640, 480 )

	glTexCoord2f(1,1);			// Texture Coordinate	(   1,   1 )
	glVertex2f(this->GetWidth(),0);				// Fourth Vertex	( 640,   0 )
	glEnd();							// Done Drawing Quads


	glEnable(GL_DEPTH_TEST);					// Enable Depth Testing
	glDisable(GL_TEXTURE_2D);					// Disable 2D Texture Mapping
	glDisable(GL_BLEND);						// Disable Blending
	glBindTexture(GL_TEXTURE_2D,0);					// Unbind The Blur Texture


	this->OnPaint();
}

void awuiForm::SetBackColor(int red, int green, int blue) {
	this->red = (red > 255?255:(red < 0?0:red));
	this->green = (green > 255?255:(green < 0?0:green));
	this->blue = (blue > 255?255:(blue < 0?0:blue));
}

void awuiForm::GetBackColor(int &red, int &green, int &blue) {
	red = this->red;
	green = this->green;
	blue = this->blue;
}

void awuiForm::ProcessEvents(ac * c) {
	const awEvent * awe;
	aw * w = this->w;
	while ((awe = awNextEvent(w))) {
		switch (awe->type) {
			case AW_EVENT_RESIZE:
				this->width = awe->u.resize.w;
				this->height = awe->u.resize.h;
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

