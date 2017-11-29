/**
 * awui/OpenGL/GL.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

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

#define OFFSET 0.5f

void GL::DrawLine(int x1, int y1, int x2, int y2) {
	int xinc = 0;
	int yinc = 0;
	int width = Math::Abs(x2 - x1);
	int height = Math::Abs(y2 - y1);

	if (width >= height) {
		if (x2 >= x1)
			xinc = 1;
		else
			xinc = -1;
	}

	if (height >= width) {
		if (y2 >= y1)
			yinc = 1;
		else
			yinc = -1;
	}

	glBegin(GL_LINES);
	glVertex2f(x1 + OFFSET, y1 + OFFSET);
	glVertex2f(x2 + xinc + OFFSET, y2 + yinc + OFFSET);
	glEnd();
}

void GL::DrawRectangle(int x1, int y1, int x2, int y2) {
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1 + OFFSET, y1 + OFFSET);
	glVertex2f(x2 + OFFSET, y1 + OFFSET);
	glVertex2f(x2 + OFFSET, y2 + OFFSET);
	glVertex2f(x1 + OFFSET, y2 + OFFSET);
	glEnd();
}

void GL::FillRectangle(int x1, int y1, int x2, int y2) {
	glBegin(GL_QUADS);
	glVertex2f(x1, y1);
	glVertex2f(x2 + 1.0f, y1);
	glVertex2f(x2 + 1.0f, y2 + 1.0f);
	glVertex2f(x1, y2 + 1.0f);
	glEnd();
}

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

void GL::DrawImageGL(awui::Drawing::Image * image, int x, int y) {
	image->Load();
	// Mas rapido guardandose solo el valor y recuperarlo despues
	GLboolean oldTexture = glIsEnabled(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);

	GLboolean oldDepth = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	GLboolean oldBlend = glIsEnabled(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, image->GetTexture());

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);glVertex2i(x, y);
	glTexCoord2f(1.0f, 0.0f);glVertex2i(x + image->GetWidth(), y);
	glTexCoord2f(1.0f, 1.0f);glVertex2i(x + image->GetWidth(), y + image->GetHeight());
	glTexCoord2f(0.0f, 1.0f);glVertex2i(x, y + image->GetHeight()) ;
	glEnd();

	if (!oldBlend)
		glDisable(GL_BLEND);
	if (oldDepth)
		glEnable(GL_DEPTH_TEST);
	if (!oldTexture)
		glDisable(GL_TEXTURE_2D);
}

void GL::DrawImageGL(awui::Drawing::Image * image, int x, int y, int width, int height) {
	image->Load();
	// Mas rapido guardandose solo el valor y recuperarlo despues
	GLboolean oldTexture = glIsEnabled(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);

	GLboolean oldDepth = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	GLboolean oldBlend = glIsEnabled(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, image->GetTexture());

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);glVertex2i(x, y);
	glTexCoord2f(1.0f, 0.0f);glVertex2i(x + width, y);
	glTexCoord2f(1.0f, 1.0f);glVertex2i(x + width, y + height);
	glTexCoord2f(0.0f, 1.0f);glVertex2i(x, y + height) ;
	glEnd();

	if (!oldBlend)
		glDisable(GL_BLEND);
	if (oldDepth)
		glEnable(GL_DEPTH_TEST);
	if (!oldTexture)
		glDisable(GL_TEXTURE_2D);
}
