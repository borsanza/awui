// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiGL.h"

#include "awuiImage.h"

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

awuiGL::awuiGL() {
}

void awuiGL::SetClippingBase(awuiRectangle rectangle) {
	this->rectangle1 = rectangle;
}

void awuiGL::SetClipping(awuiRectangle rectangle) {
	this->rectangle2 = rectangle;
}

awuiRectangle awuiGL::GetClippingBase() {
	return this->rectangle1;
}

awuiRectangle awuiGL::GetClipping() {
	return this->rectangle2;
}

awuiRectangle awuiGL::GetClippingResult() {
	return awuiRectangle::Intersect(this->rectangle1, this->rectangle2);
}

void awuiGL::SetClipping() {
	awuiRectangle rect = this->GetClippingResult();

	glScissor(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
}

void awuiGL::DrawImageGL(awuiImage * image, float x, float y) {
	static GLuint texture1;
	glDisable (GL_DEPTH_TEST);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glGenTextures(1,&texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->GetWidth(), image->GetHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image->image);

	glPushMatrix();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);glVertex2f(x, y);
	glTexCoord2f(1.0f, 0.0f);glVertex2f(x + image->GetWidth(), y);
	glTexCoord2f(1.0f, 1.0f);glVertex2f(x + image->GetWidth(), y + image->GetHeight());
	glTexCoord2f(0.0f, 1.0f);glVertex2f(x, y + image->GetHeight()) ;
	glEnd();

	glDeleteTextures(1, &texture1);
	
	glPopMatrix();
}