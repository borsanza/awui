// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Bitmap.h"
#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Math.h>

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
	this->texture = -1;

	this->stretchMode = StretchMode::Stretch;
}

Bitmap::~Bitmap() {
	glDeleteTextures(1, &this->texture);
}

int Bitmap::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Bitmap)
		return 1;

	return Control::IsClass(objectClass);
}

void Bitmap::SetStretchMode(StretchMode::Enum stretchMode) {
	this->stretchMode = stretchMode;
}

StretchMode::Enum Bitmap::GetStretchMode() {
	return this->stretchMode;
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

void Bitmap::PaintNoResized() {
	int x1, x2, y1, y2;
	x1 = Math::Round((this->GetWidth() - this->textureWidth) / 2.0f);
	y1 = Math::Round((this->GetHeight() - this->textureHeight) / 2.0f);
	x2 = x1 + this->textureWidth;
	y2 = y1 + this->textureHeight;
	glTexCoord2f(0.0f, 0.0f); glVertex2i(x1, y1); // Left Top
	glTexCoord2f(1.0f, 0.0f); glVertex2i(x2, y1); // Right Top
	glTexCoord2f(1.0f, 1.0f); glVertex2i(x2, y2); // Right Bottom
	glTexCoord2f(0.0f, 1.0f); glVertex2i(x1, y2); // Left Bottom
}

void Bitmap::PaintTiled() {
	int x0, x1, x2, x3;
	int y0, y1, y2, y3;
	float tx0, tx1, tx2, tx3;
	float ty0, ty1, ty2, ty3;

	x0 = 0;
	x1 = this->fixX1;
	x2 = this->GetWidth() - this->fixX2;
	x3 = this->GetWidth();
	y0 = 0;
	y1 = this->fixY1;
	y2 = this->GetHeight() - this->fixY2;
	y3 = this->GetHeight();
	tx0 = 0.0f;
	tx1 = (float)(this->fixX1) / (float)this->textureWidth;
	tx2 = (float)(this->textureWidth - this->fixX2) / (float)this->textureWidth;
	tx3 = 1.0f;
	ty0 = 0.0f;
	ty1 = (float)this->fixY1 / (float)this->textureHeight;
	ty2 = (float)(this->textureHeight - this->fixY2) / (float)this->textureHeight;
	ty3 = 1.0f;

// 0 | 1 | 2
//-----------
// 3 | 4 | 5
//-----------
// 6 | 7 | 8

	// 0
	if ((this->fixX1 > 0) && (this->fixY1 > 0)) {
		glTexCoord2f(tx0, ty0); glVertex2i(x0, y0); // Left Top
		glTexCoord2f(tx1, ty0); glVertex2i(x1, y0); // Right Top
		glTexCoord2f(tx1, ty1); glVertex2i(x1, y1); // Right Bottom
		glTexCoord2f(tx0, ty1); glVertex2i(x0, y1); // Left Bottom
	}

	// 2
	if ((this->fixX2 > 0) && (this->fixY1 > 0)) {
		glTexCoord2f(tx2, ty0); glVertex2i(x2, y0); // Left Top
		glTexCoord2f(tx3, ty0); glVertex2i(x3, y0); // Right Top
		glTexCoord2f(tx3, ty1); glVertex2i(x3, y1); // Right Bottom
		glTexCoord2f(tx2, ty1); glVertex2i(x2, y1); // Left Bottom
	}

	// 6
	if ((this->fixX1 > 0) && (this->fixY2 > 0)) {
		glTexCoord2f(tx0, ty2); glVertex2i(x0, y2); // Left Top
		glTexCoord2f(tx1, ty2); glVertex2i(x1, y2); // Right Top
		glTexCoord2f(tx1, ty3); glVertex2i(x1, y3); // Right Bottom
		glTexCoord2f(tx0, ty3); glVertex2i(x0, y3); // Left Bottom
	}

	// 8
	if ((this->fixX2 > 0) && (this->fixY2 > 0)) {
		glTexCoord2f(tx2, ty2); glVertex2i(x2, y2); // Left Top
		glTexCoord2f(tx3, ty2); glVertex2i(x3, y2); // Right Top
		glTexCoord2f(tx3, ty3); glVertex2i(x3, y3); // Right Bottom
		glTexCoord2f(tx2, ty3); glVertex2i(x2, y3); // Left Bottom
	}

	// 1
	if (this->fixY1 > 0) {
		int xn1 = this->fixX1;
		int xn2 = xn1 + (this->textureWidth - this->fixX1 - this->fixX2);
		while (xn2 < (this->GetWidth() - this->fixX2)) {
			glTexCoord2f(tx1, ty0); glVertex2i(xn1, y0); // Left Top
			glTexCoord2f(tx2, ty0); glVertex2i(xn2, y0); // Right Top
			glTexCoord2f(tx2, ty1); glVertex2i(xn2, y1); // Right Bottom
			glTexCoord2f(tx1, ty1); glVertex2i(xn1, y1); // Left Bottom
			xn1 = xn2;
			xn2 = xn1 + (this->textureWidth - this->fixX1 - this->fixX2);
		}
	}

	// 7
	if (this->fixY2 > 0) {
		int xn1 = this->fixX1;
		int xn2 = xn1 + (this->textureWidth - this->fixX1 - this->fixX2);
		while (xn2 < (this->GetWidth() - this->fixX2)) {
			glTexCoord2f(tx1, ty2); glVertex2i(xn1, y2); // Left Top
			glTexCoord2f(tx2, ty2); glVertex2i(xn2, y2); // Right Top
			glTexCoord2f(tx2, ty3); glVertex2i(xn2, y3); // Right Bottom
			glTexCoord2f(tx1, ty3); glVertex2i(xn1, y3); // Left Bottom
			xn1 = xn2;
			xn2 = xn1 + (this->textureWidth - this->fixX1 - this->fixX2);
		}
	}

	// 3
	if (this->fixX1 > 0) {
		int yn1 = this->fixY1;
		int yn2 = yn1 + (this->textureHeight - this->fixY1 - this->fixY2);
		while (yn2 < (this->GetHeight() - this->fixY2)) {
			glTexCoord2f(tx0, ty1); glVertex2i(x0, yn1); // Left Top
			glTexCoord2f(tx1, ty1); glVertex2i(x1, yn1); // Right Top
			glTexCoord2f(tx1, ty2); glVertex2i(x1, yn2); // Right Bottom
			glTexCoord2f(tx0, ty2); glVertex2i(x0, yn2); // Left Bottom
			yn1 = yn2;
			yn2 = yn1 + (this->textureHeight - this->fixY1 - this->fixY2);
		}
	}

	// 5
	if (this->fixX2 > 0) {
		int yn1 = this->fixY1;
		int yn2 = yn1 + (this->textureHeight - this->fixY1 - this->fixY2);
		while (yn2 < (this->GetHeight() - this->fixY2)) {
			glTexCoord2f(tx2, ty1); glVertex2i(x2, yn1); // Left Top
			glTexCoord2f(tx3, ty1); glVertex2i(x3, yn1); // Right Top
			glTexCoord2f(tx3, ty2); glVertex2i(x3, yn2); // Right Bottom
			glTexCoord2f(tx2, ty2); glVertex2i(x2, yn2); // Left Bottom
			yn1 = yn2;
			yn2 = yn1 + (this->textureHeight - this->fixY1 - this->fixY2);
		}
	}

	// 4
	int yn1 = this->fixY1;
	int yn2 = yn1 + (this->textureHeight - this->fixY1 - this->fixY2);
	while (yn2 < (this->GetHeight() - this->fixY2)) {
		int xn1 = this->fixX1;
		int xn2 = xn1 + (this->textureWidth - this->fixX1 - this->fixX2);
		while (xn2 < (this->GetWidth() - this->fixX2)) {
			glTexCoord2f(tx1, ty1); glVertex2i(xn1, yn1); // Left Top
			glTexCoord2f(tx2, ty1); glVertex2i(xn2, yn1); // Right Top
			glTexCoord2f(tx2, ty2); glVertex2i(xn2, yn2); // Right Bottom
			glTexCoord2f(tx1, ty2); glVertex2i(xn1, yn2); // Left Bottom
			xn1 = xn2;
			xn2 = xn1 + (this->textureWidth - this->fixX1 - this->fixX2);
		}
		yn1 = yn2;
		yn2 = yn1 + (this->textureHeight - this->fixY1 - this->fixY2);
	}
}

void Bitmap::PaintStretched() {
	int x0, x1, x2, x3;
	int y0, y1, y2, y3;
	float tx0, tx1, tx2, tx3;
	float ty0, ty1, ty2, ty3;

	x0 = 0;
	x1 = this->fixX1;
	x2 = this->GetWidth() - this->fixX2;
	x3 = this->GetWidth();
	y0 = 0;
	y1 = this->fixY1;
	y2 = this->GetHeight() - this->fixY2;
	y3 = this->GetHeight();
	tx0 = 0.0f;
	tx1 = (float)(this->fixX1) / (float)this->textureWidth;
	tx2 = (float)(this->textureWidth - this->fixX2) / (float)this->textureWidth;
	tx3 = 1.0f;
	ty0 = 0.0f;
	ty1 = (float)this->fixY1 / (float)this->textureHeight;
	ty2 = (float)(this->textureHeight - this->fixY2) / (float)this->textureHeight;
	ty3 = 1.0f;

// 0 | 1 | 2
//-----------
// 3 | 4 | 5
//-----------
// 6 | 7 | 8

	// 0
	if ((this->fixX1 > 0) && (this->fixY1 > 0)) {
		glTexCoord2f(tx0, ty0); glVertex2i(x0, y0); // Left Top
		glTexCoord2f(tx1, ty0); glVertex2i(x1, y0); // Right Top
		glTexCoord2f(tx1, ty1); glVertex2i(x1, y1); // Right Bottom
		glTexCoord2f(tx0, ty1); glVertex2i(x0, y1); // Left Bottom
	}

	// 1
	if (this->fixY1 > 0) {
		glTexCoord2f(tx1, ty0); glVertex2i(x1, y0); // Left Top
		glTexCoord2f(tx2, ty0); glVertex2i(x2, y0); // Right Top
		glTexCoord2f(tx2, ty1); glVertex2i(x2, y1); // Right Bottom
		glTexCoord2f(tx1, ty1); glVertex2i(x1, y1); // Left Bottom
	}

	// 2
	if ((this->fixX2 > 0) && (this->fixY1 > 0)) {
		glTexCoord2f(tx2, ty0); glVertex2i(x2, y0); // Left Top
		glTexCoord2f(tx3, ty0); glVertex2i(x3, y0); // Right Top
		glTexCoord2f(tx3, ty1); glVertex2i(x3, y1); // Right Bottom
		glTexCoord2f(tx2, ty1); glVertex2i(x2, y1); // Left Bottom
	}

	// 3
	if (this->fixX1 > 0) {
		glTexCoord2f(tx0, ty1); glVertex2i(x0, y1); // Left Top
		glTexCoord2f(tx1, ty1); glVertex2i(x1, y1); // Right Top
		glTexCoord2f(tx1, ty2); glVertex2i(x1, y2); // Right Bottom
		glTexCoord2f(tx0, ty2); glVertex2i(x0, y2); // Left Bottom
	}

	// 4
	glTexCoord2f(tx1, ty1); glVertex2i(x1, y1); // Left Top
	glTexCoord2f(tx2, ty1); glVertex2i(x2, y1); // Right Top
	glTexCoord2f(tx2, ty2); glVertex2i(x2, y2); // Right Bottom
	glTexCoord2f(tx1, ty2); glVertex2i(x1, y2); // Left Bottom

	// 5
	if (this->fixX2 > 0) {
		glTexCoord2f(tx2, ty1); glVertex2i(x2, y1); // Left Top
		glTexCoord2f(tx3, ty1); glVertex2i(x3, y1); // Right Top
		glTexCoord2f(tx3, ty2); glVertex2i(x3, y2); // Right Bottom
		glTexCoord2f(tx2, ty2); glVertex2i(x2, y2); // Left Bottom
	}

	// 6
	if ((this->fixX1 > 0) && (this->fixY2 > 0)) {
		glTexCoord2f(tx0, ty2); glVertex2i(x0, y2); // Left Top
		glTexCoord2f(tx1, ty2); glVertex2i(x1, y2); // Right Top
		glTexCoord2f(tx1, ty3); glVertex2i(x1, y3); // Right Bottom
		glTexCoord2f(tx0, ty3); glVertex2i(x0, y3); // Left Bottom
	}

	// 7
	if (this->fixY2 > 0) {
		glTexCoord2f(tx1, ty2); glVertex2i(x1, y2); // Left Top
		glTexCoord2f(tx2, ty2); glVertex2i(x2, y2); // Right Top
		glTexCoord2f(tx2, ty3); glVertex2i(x2, y3); // Right Bottom
		glTexCoord2f(tx1, ty3); glVertex2i(x1, y3); // Left Bottom
	}

	// 8
	if ((this->fixX2 > 0) && (this->fixY2 > 0)) {
		glTexCoord2f(tx2, ty2); glVertex2i(x2, y2); // Left Top
		glTexCoord2f(tx3, ty2); glVertex2i(x3, y2); // Right Top
		glTexCoord2f(tx3, ty3); glVertex2i(x3, y3); // Right Bottom
		glTexCoord2f(tx2, ty3); glVertex2i(x2, y3); // Left Bottom
	}
}

void Bitmap::OnPaint(GL* gl) {
	this->Load();

	if ((this->textureWidth == 0) || (this->textureHeight == 0))
		return;

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

	glBegin(GL_QUADS);

	switch (this->stretchMode) {
		case StretchMode::Enum::NoResize:
			this->PaintNoResized();
			break;
		case StretchMode::Enum::Tile:
			this->PaintTiled();
			break;
		case StretchMode::Enum::Stretch:
			this->PaintStretched();
			break;
	}

	glEnd();
	glPopMatrix();

	if (!oldBlend)
		glDisable(GL_BLEND);
	if (oldDepth)
		glEnable(GL_DEPTH_TEST);
	if (!oldTexture)
		glDisable(GL_TEXTURE_2D);
}
