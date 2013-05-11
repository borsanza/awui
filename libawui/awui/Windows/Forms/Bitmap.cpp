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

	this->fixX1 = -1;
	this->fixX2 = -1;
	this->fixY1 = -1;
	this->fixY2 = -1;
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, textureImage->w, textureImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage->pixels);
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

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f( 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f( 97.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f( 97.0f, 97.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex2f( 0.0f, 97.0f);
	glEnd();
	glPopMatrix();

	if (!oldTexture)
		glDisable(GL_TEXTURE_2D);
	if (!oldBlend)
		glDisable(GL_BLEND);
	if (oldDepth)
		glEnable(GL_DEPTH_TEST);
}
