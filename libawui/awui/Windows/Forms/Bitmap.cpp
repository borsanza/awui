// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Bitmap.h"
#include <awui/Console.h>
#include <awui/Convert.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

Bitmap::Bitmap(const String file) {
	this->loaded = false;
	this->file = file;

	this->fixX1 = 0;
	this->fixX2 = 0;
	this->fixY1 = 0;
	this->fixY2 = 0;
	this->textureWidth = 0;
	this->textureHeight = 0;
}

Bitmap::~Bitmap() {
	glDeleteTextures(1, &this->texture);
}

int Bitmap::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Bitmap)
		return 1;

	return Control::IsClass(objectClass);
}

void Bitmap::SetFixedMargins(int x1, int y1, int x2, int y2) {
	this->fixX1 = x1;
	this->fixY1 = y1;
	this->fixX2 = x2;
	this->fixY2 = y2;
}

void Bitmap::Load() {
	if (this->loaded)
		return;

	this->loaded = true;

	SDL_Surface * textureImage = IMG_Load(file.ToCharArray());
	if (textureImage) {
		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST or GL_LINEAR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST or GL_LINEAR
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, textureImage->w, textureImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage->pixels);
		this->textureWidth = textureImage->w;
		this->textureHeight = textureImage->h;
	}

	if (textureImage)
		SDL_FreeSurface(textureImage);
}

void Bitmap::OnPaint(GL* gl) {
	this->Load();

	GLboolean oldTexture = glIsEnabled(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);

	GLboolean oldDepth = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	GLboolean oldBlend = glIsEnabled(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, this->texture);

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);

	int x0, x1, x2, x3;
	int y0, y1, y2, y3;
	float tx0, tx1, tx2, tx3;
	float ty0, ty1, ty2, ty3;

	glBegin(GL_QUADS);

	x0 = 0;
	x1 = this->fixX1;
	x2 = this->GetWidth() - this->fixX2;
	x3 = this->GetWidth();
	y0 = 0;
	y1 = this->fixY1;
	y2 = this->GetHeight() - this->fixY2;
	y3 = this->GetHeight();
	tx0 = 0.0f;
	tx1 = (float)this->fixX1 / (float)this->textureWidth;
	tx2 = (float)(this->textureWidth - this->fixX2) / (float)this->textureWidth;
	tx3 = 1.0f;
	ty0 = 0.0f;
	ty1 = (float)this->fixY1 / (float)this->textureHeight;
	ty2 = (float)(this->textureHeight - this->fixY2) / (float)this->textureHeight;
	ty3 = 1.0f;

	if (this->fixX1 > 0) {
		glTexCoord2f(tx0, ty1); glVertex2i(x0, y1); // Left Top
		glTexCoord2f(tx1, ty1); glVertex2i(x1, y1); // Right Top
		glTexCoord2f(tx1, ty2); glVertex2i(x1, y2); // Right Bottom
		glTexCoord2f(tx0, ty2); glVertex2i(x0, y2); // Left Bottom
	}

	if (this->fixY1 > 0) {
		glTexCoord2f(tx1, ty0); glVertex2i(x1, y0); // Left Top
		glTexCoord2f(tx2, ty0); glVertex2i(x2, y0); // Right Top
		glTexCoord2f(tx2, ty1); glVertex2i(x2, y1); // Right Bottom
		glTexCoord2f(tx1, ty1); glVertex2i(x1, y1); // Left Bottom
	}

	if (this->fixX2 > 0) {
		glTexCoord2f(tx2, ty1); glVertex2i(x2, y1); // Left Top
		glTexCoord2f(tx3, ty1); glVertex2i(x3, y1); // Right Top
		glTexCoord2f(tx3, ty2); glVertex2i(x3, y2); // Right Bottom
		glTexCoord2f(tx2, ty2); glVertex2i(x2, y2); // Left Bottom
	}

	if (this->fixY2 > 0) {
		glTexCoord2f(tx1, ty2); glVertex2i(x1, y2); // Left Top
		glTexCoord2f(tx2, ty2); glVertex2i(x2, y2); // Right Top
		glTexCoord2f(tx2, ty3); glVertex2i(x2, y3); // Right Bottom
		glTexCoord2f(tx1, ty3); glVertex2i(x1, y3); // Left Bottom
	}

	if ((this->fixX1 > 0) && (this->fixY1 > 0)) {
		glTexCoord2f(tx0, ty0); glVertex2i(x0, y0); // Left Top
		glTexCoord2f(tx1, ty0); glVertex2i(x1, y0); // Right Top
		glTexCoord2f(tx1, ty1); glVertex2i(x1, y1); // Right Bottom
		glTexCoord2f(tx0, ty1); glVertex2i(x0, y1); // Left Bottom
	}

	if ((this->fixX2 > 0) && (this->fixY1 > 0)) {
		glTexCoord2f(tx2, ty0); glVertex2i(x2, y0); // Left Top
		glTexCoord2f(tx3, ty0); glVertex2i(x3, y0); // Right Top
		glTexCoord2f(tx3, ty1); glVertex2i(x3, y1); // Right Bottom
		glTexCoord2f(tx2, ty1); glVertex2i(x2, y1); // Left Bottom
	}

	if ((this->fixX1 > 0) && (this->fixY2 > 0)) {
		glTexCoord2f(tx0, ty2); glVertex2i(x0, y2); // Left Top
		glTexCoord2f(tx1, ty2); glVertex2i(x1, y2); // Right Top
		glTexCoord2f(tx1, ty3); glVertex2i(x1, y3); // Right Bottom
		glTexCoord2f(tx0, ty3); glVertex2i(x0, y3); // Left Bottom
	}

	if ((this->fixX2 > 0) && (this->fixY2 > 0)) {
		glTexCoord2f(tx2, ty2); glVertex2i(x2, y2); // Left Top
		glTexCoord2f(tx3, ty2); glVertex2i(x3, y2); // Right Top
		glTexCoord2f(tx3, ty3); glVertex2i(x3, y3); // Right Bottom
		glTexCoord2f(tx2, ty3); glVertex2i(x2, y3); // Left Bottom
	}

	glTexCoord2f(tx1, ty1); glVertex2i(x1, y1); // Left Top
	glTexCoord2f(tx2, ty1); glVertex2i(x2, y1); // Right Top
	glTexCoord2f(tx2, ty2); glVertex2i(x2, y2); // Right Bottom
	glTexCoord2f(tx1, ty2); glVertex2i(x1, y2); // Left Bottom
/*
	glTexCoord2f(0, 0); glVertex2i(0, 0); // Left Top
	glTexCoord2f(1, 0); glVertex2i(97, 0); // Right Top
	glTexCoord2f(1, 1); glVertex2i(97, 97); // Right Bottom
	glTexCoord2f(0, 1); glVertex2i(0, 97); // Left Bottom
*/
	glEnd();
	glPopMatrix();

	if (!oldBlend)
		glDisable(GL_BLEND);
	if (oldDepth)
		glEnable(GL_DEPTH_TEST);
	if (!oldTexture)
		glDisable(GL_TEXTURE_2D);
}

// StretchMode - NoResize, Tile, Stretch
