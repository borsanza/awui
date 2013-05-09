// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "GL.h"

#include <awui/Drawing/Image.h>
#include <awui/Math.h>
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

void GL::DrawLine(int x, int y, int x2, int y2) {
	int xinc = 0;
	int yinc = 0;
	int width = Math::Abs(x2 - x);
	int height = Math::Abs(y2 - y);

	if (width >= height) {
		if (x2 >= x)
			xinc = 1;
		else
			xinc = -1;
	}

	if (height >= width) {
		if (y2 >= y)
			yinc = 1;
		else
			yinc = -1;
	}

	glBegin(GL_LINES);
	glVertex2i(x, y);
	glVertex2i(x2 + xinc, y2 + yinc);
	glEnd();
}

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

void GL::DrawImageGL(awui::Drawing::Image * image, float x, float y) {
	// Mas rapido guardandose solo el valor y recuperarlo despues
	GLboolean oldDepth = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	GLboolean oldBlend = glIsEnabled(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPixelZoom(1.0f, -1.0f);
	glRasterPos2f(x, y);
	GLboolean valid;
	glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID,&valid);
	if (valid) {
		glDrawPixels(image->GetWidth(), image->GetHeight(), GL_BGRA, GL_UNSIGNED_BYTE, image->image);
	} else {
		static GLuint texture1;

		glGenTextures(1, &texture1);
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
	}

	if (!oldBlend)
		glDisable(GL_BLEND);
	if (oldDepth)
		glEnable(GL_BLEND);
}
