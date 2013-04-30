// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "GL.h"

#include <awui/Drawing/Image.h>
#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;

GL::GL() {
}

void GL::SetClippingBase(Rectangle rectangle) {
	this->rectangle1 = rectangle;
}

void GL::SetClipping(Rectangle rectangle) {
	this->rectangle2 = rectangle;
}

Rectangle GL::GetClippingBase() {
	return this->rectangle1;
}

Rectangle GL::GetClipping() {
	return this->rectangle2;
}

Rectangle GL::GetClippingResult() {
	return Rectangle::Intersect(this->rectangle1, this->rectangle2);
}

void GL::SetClipping() {
	Rectangle rect = this->GetClippingResult();

	glScissor(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
}

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

void GL::DrawImageGL(awui::Drawing::Image * image, float x, float y) {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	static GLuint texture1;

	glGenTextures(1,&texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->GetWidth(), image->GetHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image->image);

	glPushMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);glVertex2f(x, y);
	glTexCoord2f(1.0f, 0.0f);glVertex2f(x + image->GetWidth(), y);
	glTexCoord2f(1.0f, 1.0f);glVertex2f(x + image->GetWidth(), y + image->GetHeight());
	glTexCoord2f(0.0f, 1.0f);glVertex2f(x, y + image->GetHeight()) ;
	glEnd();

	glDeleteTextures(1, &texture1);

	glPopMatrix();
/*
	glRasterPos2f(x, y + image->GetHeight());
	glDrawPixels(image->GetWidth(), image->GetHeight(), GL_BGRA, GL_UNSIGNED_BYTE, image->image);
	glDisable(GL_BLEND);
*/
}
