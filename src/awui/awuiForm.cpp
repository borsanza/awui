// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiForm.h"

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

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

void awuiForm::SetTop(int y) {
	this->SetLocation(this->x, y);
}

void awuiForm::SetLeft(int x) {
	this->SetLocation(x, this->y);
}

void awuiForm::SetLocation(int x, int y) {
	this->SetBounds(x, y, this->width, this->height);
}

void awuiForm::SetWidth(int width) {
	this->SetSize(width, this->height);
}

void awuiForm::SetHeight(int height) {
	this->SetSize(this->width, height);
}

void awuiForm::SetSize(int width, int height) {
	this->SetBounds(this->x, this->y, width, height);
}

void awuiForm::SetBounds(int x, int y, int width, int height) {
	if ((this->x == x) && (this->y == y) && (this->width == width) && (this->height == height))
		return;
}

int awuiForm::GetTop() {
	return this->y;
}

int awuiForm::GetLeft() {
	return this->x;
}

void awuiForm::GetLocation(int &x, int &y) {
	x = this->GetLeft();
	y = this->GetTop();
}

int awuiForm::GetWidth() {
	return this->width;
}

int awuiForm::GetHeight() {
	return this->height;
}

void awuiForm::GetSize(int &width, int &height) {
	width = this->GetWidth();
	height = this->GetHeight();
}

void awuiForm::GetBounds(int &x, int &y, int &width, int &height) {
	this->GetLocation(x, y);
	this->GetSize(width, height);
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

	glClearColor(this->red / 255.0, this->green / 255.0, this->blue / 255.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

