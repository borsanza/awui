// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Bitmap.h"

#include <SDL_image.h>
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>
#include <awui/Console.h>
#include <awui/Math.h>

using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

ArrayList Bitmap::list;

Bitmap::Bitmap(const String file) {
	m_class = Classes::Bitmap;
	m_loaded = false;
	m_file = file;

	m_fixX1 = 0;
	m_fixX2 = 0;
	m_fixY1 = 0;
	m_fixY2 = 0;
	m_textureWidth = 0;
	m_textureHeight = 0;
	m_texture = -1;
	m_color = ColorF::FromArgb(1.0f, 1.0f, 1.0f, 1.0f);

	m_stretchMode = StretchMode::Stretch;

	Bitmap::list.Add(this);
}

Bitmap::~Bitmap() {
	Bitmap::list.Remove(this);

	Unload();
}

bool Bitmap::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Bitmap) || Control::IsClass(objectClass);
}

void Bitmap::SetStretchMode(StretchMode::Enum stretchMode) {
	m_stretchMode = stretchMode;
}

StretchMode::Enum Bitmap::GetStretchMode() {
	return m_stretchMode;
}

void Bitmap::SetFixedMargins(int x1, int y1, int x2, int y2) {
	m_fixX1 = x1;
	m_fixY1 = y1;
	m_fixX2 = x2;
	m_fixY2 = y2;
}

void Bitmap::GetFixedMargins(int *x1, int *y1, int *x2, int *y2) {
	*x1 = m_fixX1;
	*y1 = m_fixY1;
	*x2 = m_fixX2;
	*y2 = m_fixY2;
}

void Bitmap::Load() {
	if (m_loaded || m_file.IsEmpty())
		return;

	SDL_Surface *textureImage = IMG_Load(m_file.ToCharArray());
	if (!textureImage) {
		Console::Error->WriteLine("Failed to load texture: %s", m_file);
		return;
	}

	SDL_Surface *optimizedImage = SDL_ConvertSurfaceFormat(textureImage, SDL_PIXELFORMAT_RGBA32, 0);
	SDL_FreeSurface(textureImage);
	if (!optimizedImage) {
		Console::Error->WriteLine("Failed to optimize texture format: %s", m_file);
		return;
	}
	textureImage = optimizedImage;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	if (glGetError() != GL_NO_ERROR) {
		Console::Error->WriteLine("OpenGL error: Failed to bind texture.");
		glDeleteTextures(1, &m_texture);
		SDL_FreeSurface(textureImage);
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLenum internalFormat = textureImage->format->BytesPerPixel == 4 ? GL_RGBA8 : GL_RGB8;
	GLenum textureFormat = textureImage->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, textureImage->w, textureImage->h, 0, textureFormat, GL_UNSIGNED_BYTE, textureImage->pixels);
	if (glGetError() != GL_NO_ERROR) {
		Console::Error->WriteLine("OpenGL error: Failed to load texture image.");
		glDeleteTextures(1, &m_texture);
		SDL_FreeSurface(textureImage);
		return;
	}

	m_textureWidth = textureImage->w;
	m_textureHeight = textureImage->h;

	SDL_FreeSurface(textureImage);

	m_loaded = true;
}

void Bitmap::Unload() {
	if (!m_loaded)
		return;

	glDeleteTextures(1, &m_texture);
	m_texture = -1;
	m_loaded = false;
}

void Bitmap::UnloadAll() {
	for (int i = 0; i < Bitmap::list.GetCount(); i++) {
		Bitmap *bitmap = (Bitmap *) Bitmap::list.Get(i);
		bitmap->Unload();
	}
}

// GL_CCW
void Bitmap::PaintNoResized() {
	int x1, x2, y1, y2;
	x1 = Math::Round((GetWidth() - m_textureWidth) / 2.0f);
	y1 = Math::Round((GetHeight() - m_textureHeight) / 2.0f);
	x2 = x1 + m_textureWidth;
	y2 = y1 + m_textureHeight;

	glTexCoord2f(0.0f, 1.0f);
	glVertex2i(x1, y2); // Left Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex2i(x2, y2); // Right Bottom
	glTexCoord2f(1.0f, 0.0f);
	glVertex2i(x2, y1); // Right Top
	glTexCoord2f(0.0f, 0.0f);
	glVertex2i(x1, y1); // Left Top
}

// GL_CCW
void Bitmap::PaintTiled() {
	int x0, x1, x2, x3;
	int y0, y1, y2, y3;
	float tx0, tx1, tx2, tx3;
	float ty0, ty1, ty2, ty3;

	x0 = 0;
	x1 = m_fixX1;
	x2 = GetWidth() - m_fixX2;
	x3 = GetWidth();
	y0 = 0;
	y1 = m_fixY1;
	y2 = GetHeight() - m_fixY2;
	y3 = GetHeight();
	tx0 = 0.0f;
	tx1 = (float) (m_fixX1) / (float) m_textureWidth;
	tx2 = (float) (m_textureWidth - m_fixX2) / (float) m_textureWidth;
	tx3 = 1.0f;
	ty0 = 0.0f;
	ty1 = (float) m_fixY1 / (float) m_textureHeight;
	ty2 = (float) (m_textureHeight - m_fixY2) / (float) m_textureHeight;
	ty3 = 1.0f;

	// 0 | 1 | 2
	//-----------
	// 3 | 4 | 5
	//-----------
	// 6 | 7 | 8

	// 0
	if ((m_fixX1 > 0) && (m_fixY1 > 0)) {
		glTexCoord2f(tx0, ty1);
		glVertex2i(x0, y1); // Left Bottom
		glTexCoord2f(tx1, ty1);
		glVertex2i(x1, y1); // Right Bottom
		glTexCoord2f(tx1, ty0);
		glVertex2i(x1, y0); // Right Top
		glTexCoord2f(tx0, ty0);
		glVertex2i(x0, y0); // Left Top
	}

	// 2
	if ((m_fixX2 > 0) && (m_fixY1 > 0)) {
		glTexCoord2f(tx2, ty1);
		glVertex2i(x2, y1); // Left Bottom
		glTexCoord2f(tx3, ty1);
		glVertex2i(x3, y1); // Right Bottom
		glTexCoord2f(tx3, ty0);
		glVertex2i(x3, y0); // Right Top
		glTexCoord2f(tx2, ty0);
		glVertex2i(x2, y0); // Left Top
	}

	// 6
	if ((m_fixX1 > 0) && (m_fixY2 > 0)) {
		glTexCoord2f(tx0, ty3);
		glVertex2i(x0, y3); // Left Bottom
		glTexCoord2f(tx1, ty3);
		glVertex2i(x1, y3); // Right Bottom
		glTexCoord2f(tx1, ty2);
		glVertex2i(x1, y2); // Right Top
		glTexCoord2f(tx0, ty2);
		glVertex2i(x0, y2); // Left Top
	}

	// 8
	if ((m_fixX2 > 0) && (m_fixY2 > 0)) {
		glTexCoord2f(tx2, ty3);
		glVertex2i(x2, y3); // Left Bottom
		glTexCoord2f(tx3, ty3);
		glVertex2i(x3, y3); // Right Bottom
		glTexCoord2f(tx3, ty2);
		glVertex2i(x3, y2); // Right Top
		glTexCoord2f(tx2, ty2);
		glVertex2i(x2, y2); // Left Top
	}

	// 1
	if (m_fixY1 > 0) {
		int xn1 = m_fixX1;
		int xn2 = xn1 + (m_textureWidth - m_fixX1 - m_fixX2);
		while (xn2 < (GetWidth() - m_fixX2)) {
			glTexCoord2f(tx1, ty1);
			glVertex2i(xn1, y1); // Left Bottom
			glTexCoord2f(tx2, ty1);
			glVertex2i(xn2, y1); // Right Bottom
			glTexCoord2f(tx2, ty0);
			glVertex2i(xn2, y0); // Right Top
			glTexCoord2f(tx1, ty0);
			glVertex2i(xn1, y0); // Left Top
			xn1 = xn2;
			xn2 = xn1 + (m_textureWidth - m_fixX1 - m_fixX2);
		}

		// Recalcular tx2
		xn2 = GetWidth() - m_fixX2;
		glTexCoord2f(tx1, ty1);
		glVertex2i(xn1, y1); // Left Bottom
		glTexCoord2f(tx2, ty1);
		glVertex2i(xn2, y1); // Right Bottom
		glTexCoord2f(tx2, ty0);
		glVertex2i(xn2, y0); // Right Top
		glTexCoord2f(tx1, ty0);
		glVertex2i(xn1, y0); // Left Top
	}

	// 3
	if (m_fixX1 > 0) {
		int yn1 = m_fixY1;
		int yn2 = yn1 + (m_textureHeight - m_fixY1 - m_fixY2);
		while (yn2 < (GetHeight() - m_fixY2)) {
			glTexCoord2f(tx0, ty2);
			glVertex2i(x0, yn2); // Left Bottom
			glTexCoord2f(tx1, ty2);
			glVertex2i(x1, yn2); // Right Bottom
			glTexCoord2f(tx1, ty1);
			glVertex2i(x1, yn1); // Right Top
			glTexCoord2f(tx0, ty1);
			glVertex2i(x0, yn1); // Left Top
			yn1 = yn2;
			yn2 = yn1 + (m_textureHeight - m_fixY1 - m_fixY2);
		}

		// Recalcular ty2
		yn2 = GetHeight() - m_fixY2;
		glTexCoord2f(tx0, ty2);
		glVertex2i(x0, yn2); // Left Bottom
		glTexCoord2f(tx1, ty2);
		glVertex2i(x1, yn2); // Right Bottom
		glTexCoord2f(tx1, ty1);
		glVertex2i(x1, yn1); // Right Top
		glTexCoord2f(tx0, ty1);
		glVertex2i(x0, yn1); // Left Top
	}

	// 5
	if (m_fixX2 > 0) {
		int yn1 = m_fixY1;
		int yn2 = yn1 + (m_textureHeight - m_fixY1 - m_fixY2);
		while (yn2 < (GetHeight() - m_fixY2)) {
			glTexCoord2f(tx2, ty2);
			glVertex2i(x2, yn2); // Left Bottom
			glTexCoord2f(tx3, ty2);
			glVertex2i(x3, yn2); // Right Bottom
			glTexCoord2f(tx3, ty1);
			glVertex2i(x3, yn1); // Right Top
			glTexCoord2f(tx2, ty1);
			glVertex2i(x2, yn1); // Left Top
			yn1 = yn2;
			yn2 = yn1 + (m_textureHeight - m_fixY1 - m_fixY2);
		}

		// Recalcular la ty2
		yn2 = GetHeight() - m_fixY2;
		glTexCoord2f(tx2, ty2);
		glVertex2i(x2, yn2); // Left Bottom
		glTexCoord2f(tx3, ty2);
		glVertex2i(x3, yn2); // Right Bottom
		glTexCoord2f(tx3, ty1);
		glVertex2i(x3, yn1); // Right Top
		glTexCoord2f(tx2, ty1);
		glVertex2i(x2, yn1); // Left Top
	}

	// 7
	if (m_fixY2 > 0) {
		int xn1 = m_fixX1;
		int xn2 = xn1 + (m_textureWidth - m_fixX1 - m_fixX2);
		while (xn2 < (GetWidth() - m_fixX2)) {
			glTexCoord2f(tx1, ty3);
			glVertex2i(xn1, y3); // Left Bottom
			glTexCoord2f(tx2, ty3);
			glVertex2i(xn2, y3); // Right Bottom
			glTexCoord2f(tx2, ty2);
			glVertex2i(xn2, y2); // Right Top
			glTexCoord2f(tx1, ty2);
			glVertex2i(xn1, y2); // Left Top
			xn1 = xn2;
			xn2 = xn1 + (m_textureWidth - m_fixX1 - m_fixX2);
		}

		// Recalcular la tx2;
		xn2 = GetWidth() - m_fixX2;
		glTexCoord2f(tx1, ty3);
		glVertex2i(xn1, y3); // Left Bottom
		glTexCoord2f(tx2, ty3);
		glVertex2i(xn2, y3); // Right Bottom
		glTexCoord2f(tx2, ty2);
		glVertex2i(xn2, y2); // Right Top
		glTexCoord2f(tx1, ty2);
		glVertex2i(xn1, y2); // Left Top
	}

	// 4
	int xn1, xn2;
	int yn1 = m_fixY1;
	int yn2 = yn1 + (m_textureHeight - m_fixY1 - m_fixY2);
	while (yn2 < (GetHeight() - m_fixY2)) {
		xn1 = m_fixX1;
		xn2 = xn1 + (m_textureWidth - m_fixX1 - m_fixX2);
		while (xn2 < (GetWidth() - m_fixX2)) {
			glTexCoord2f(tx1, ty2);
			glVertex2i(xn1, yn2); // Left Bottom
			glTexCoord2f(tx2, ty2);
			glVertex2i(xn2, yn2); // Right Bottom
			glTexCoord2f(tx2, ty1);
			glVertex2i(xn2, yn1); // Right Top
			glTexCoord2f(tx1, ty1);
			glVertex2i(xn1, yn1); // Left Top
			xn1 = xn2;
			xn2 = xn1 + (m_textureWidth - m_fixX1 - m_fixX2);
		}
		yn1 = yn2;
		yn2 = yn1 + (m_textureHeight - m_fixY1 - m_fixY2);
	}

	// Recalcular ty2
	xn1 = m_fixX1;
	xn2 = xn1 + (m_textureWidth - m_fixX1 - m_fixX2);
	yn2 = GetHeight() - m_fixY2;
	while (xn2 < (GetWidth() - m_fixX2)) {
		glTexCoord2f(tx1, ty2);
		glVertex2i(xn1, yn2); // Left Bottom
		glTexCoord2f(tx2, ty2);
		glVertex2i(xn2, yn2); // Right Bottom
		glTexCoord2f(tx2, ty1);
		glVertex2i(xn2, yn1); // Right Top
		glTexCoord2f(tx1, ty1);
		glVertex2i(xn1, yn1); // Left Top
		xn1 = xn2;
		xn2 = xn1 + (m_textureWidth - m_fixX1 - m_fixX2);
	}

	// Recalcular tx2
	yn1 = m_fixY1;
	yn2 = yn1 + (m_textureHeight - m_fixY1 - m_fixY2);
	xn2 = GetWidth() - m_fixX2;
	while (yn2 < (GetHeight() - m_fixY2)) {
		glTexCoord2f(tx1, ty2);
		glVertex2i(xn1, yn2); // Left Bottom
		glTexCoord2f(tx2, ty2);
		glVertex2i(xn2, yn2); // Right Bottom
		glTexCoord2f(tx2, ty1);
		glVertex2i(xn2, yn1); // Right Top
		glTexCoord2f(tx1, ty1);
		glVertex2i(xn1, yn1); // Left Top
		yn1 = yn2;
		yn2 = yn1 + (m_textureHeight - m_fixY1 - m_fixY2);
	}

	// Recalcular la tx2 y la ty2
	xn2 = GetWidth() - m_fixX2;
	yn2 = GetHeight() - m_fixY2;
	glTexCoord2f(tx1, ty2);
	glVertex2i(xn1, yn2); // Left Bottom
	glTexCoord2f(tx2, ty2);
	glVertex2i(xn2, yn2); // Right Bottom
	glTexCoord2f(tx2, ty1);
	glVertex2i(xn2, yn1); // Right Top
	glTexCoord2f(tx1, ty1);
	glVertex2i(xn1, yn1); // Left Top
}

// GL_CCW
void Bitmap::PaintTexture(int left, int top, int right, int bottom) {
	int x0, x1, x2, x3;
	int y0, y1, y2, y3;
	float tx0, tx1, tx2, tx3;
	float ty0, ty1, ty2, ty3;

	x0 = left;
	x1 = left + m_fixX1;
	x2 = right - m_fixX2;
	x3 = right;
	y0 = top;
	y1 = top + m_fixY1;
	y2 = bottom - m_fixY2;
	y3 = bottom;
	tx0 = 0.0f;
	tx1 = (float) (m_fixX1) / (float) m_textureWidth;
	tx2 = (float) (m_textureWidth - m_fixX2) / (float) m_textureWidth;
	tx3 = 1.0f;
	ty0 = 0.0f;
	ty1 = (float) m_fixY1 / (float) m_textureHeight;
	ty2 = (float) (m_textureHeight - m_fixY2) / (float) m_textureHeight;
	ty3 = 1.0f;

	// 0 | 1 | 2
	//-----------
	// 3 | 4 | 5
	//-----------
	// 6 | 7 | 8

	// 0
	if ((m_fixX1 > 0) && (m_fixY1 > 0)) {
		glTexCoord2f(tx0, ty1);
		glVertex2i(x0, y1); // Left Bottom
		glTexCoord2f(tx1, ty1);
		glVertex2i(x1, y1); // Right Bottom
		glTexCoord2f(tx1, ty0);
		glVertex2i(x1, y0); // Right Top
		glTexCoord2f(tx0, ty0);
		glVertex2i(x0, y0); // Left Top
	}

	// 1
	if (m_fixY1 > 0) {
		glTexCoord2f(tx1, ty1);
		glVertex2i(x1, y1); // Left Bottom
		glTexCoord2f(tx2, ty1);
		glVertex2i(x2, y1); // Right Bottom
		glTexCoord2f(tx2, ty0);
		glVertex2i(x2, y0); // Right Top
		glTexCoord2f(tx1, ty0);
		glVertex2i(x1, y0); // Left Top
	}

	// 2
	if ((m_fixX2 > 0) && (m_fixY1 > 0)) {
		glTexCoord2f(tx2, ty1);
		glVertex2i(x2, y1); // Left Bottom
		glTexCoord2f(tx3, ty1);
		glVertex2i(x3, y1); // Right Bottom
		glTexCoord2f(tx3, ty0);
		glVertex2i(x3, y0); // Right Top
		glTexCoord2f(tx2, ty0);
		glVertex2i(x2, y0); // Left Top
	}

	// 3
	if (m_fixX1 > 0) {
		glTexCoord2f(tx0, ty2);
		glVertex2i(x0, y2); // Left Bottom
		glTexCoord2f(tx1, ty2);
		glVertex2i(x1, y2); // Right Bottom
		glTexCoord2f(tx1, ty1);
		glVertex2i(x1, y1); // Right Top
		glTexCoord2f(tx0, ty1);
		glVertex2i(x0, y1); // Left Top
	}

	// 4
	glTexCoord2f(tx1, ty2);
	glVertex2i(x1, y2); // Left Bottom
	glTexCoord2f(tx2, ty2);
	glVertex2i(x2, y2); // Right Bottom
	glTexCoord2f(tx2, ty1);
	glVertex2i(x2, y1); // Right Top
	glTexCoord2f(tx1, ty1);
	glVertex2i(x1, y1); // Left Top

	// 5
	if (m_fixX2 > 0) {
		glTexCoord2f(tx2, ty2);
		glVertex2i(x2, y2); // Left Bottom
		glTexCoord2f(tx3, ty2);
		glVertex2i(x3, y2); // Right Bottom
		glTexCoord2f(tx3, ty1);
		glVertex2i(x3, y1); // Right Top
		glTexCoord2f(tx2, ty1);
		glVertex2i(x2, y1); // Left Top
	}

	// 6
	if ((m_fixX1 > 0) && (m_fixY2 > 0)) {
		glTexCoord2f(tx0, ty3);
		glVertex2i(x0, y3); // Left Bottom
		glTexCoord2f(tx1, ty3);
		glVertex2i(x1, y3); // Right Bottom
		glTexCoord2f(tx1, ty2);
		glVertex2i(x1, y2); // Right Top
		glTexCoord2f(tx0, ty2);
		glVertex2i(x0, y2); // Left Top
	}

	// 7
	if (m_fixY2 > 0) {
		glTexCoord2f(tx1, ty3);
		glVertex2i(x1, y3); // Left Bottom
		glTexCoord2f(tx2, ty3);
		glVertex2i(x2, y3); // Right Bottom
		glTexCoord2f(tx2, ty2);
		glVertex2i(x2, y2); // Right Top
		glTexCoord2f(tx1, ty2);
		glVertex2i(x1, y2); // Left Top
	}

	// 8
	if ((m_fixX2 > 0) && (m_fixY2 > 0)) {
		glTexCoord2f(tx2, ty3);
		glVertex2i(x2, y3); // Left Bottom
		glTexCoord2f(tx3, ty3);
		glVertex2i(x3, y3); // Right Bottom
		glTexCoord2f(tx3, ty2);
		glVertex2i(x3, y2); // Right Top
		glTexCoord2f(tx2, ty2);
		glVertex2i(x2, y2); // Left Top
	}
}

void Bitmap::PaintStretched() {
	PaintTexture(0, 0, GetRight(), GetBottom());
}

void Bitmap::PaintAspectFit() {
	// Obtener dimensiones del área de destino
	int destWidth = GetRight();
	int destHeight = GetBottom();

	// Calcular la relación de aspecto de la imagen y del destino
	float imageAspectRatio = static_cast<float>(m_textureWidth) / m_textureHeight;
	float destAspectRatio = static_cast<float>(destWidth) / destHeight;

	int drawWidth, drawHeight;

	if (destAspectRatio > imageAspectRatio) {
		// El área de destino es más ancha en proporción a la imagen: ajustar por altura
		drawHeight = destHeight;
		drawWidth = static_cast<int>(imageAspectRatio * drawHeight);
	} else {
		// El área de destino es más alta en proporción, o igual: ajustar por ancho
		drawWidth = destWidth;
		drawHeight = static_cast<int>(drawWidth / imageAspectRatio);
	}

	// Calcular posición inicial para centrar la imagen
	int left = (destWidth - drawWidth) / 2;
	int top = (destHeight - drawHeight) / 2;
	int right = left + drawWidth;
	int bottom = top + drawHeight;

	// Pintar la textura ajustada y centrada
	PaintTexture(left, top, right, bottom);
}

void Bitmap::PaintAspectFill() {
	// Obtener dimensiones del área de destino
	int destWidth = GetRight();
	int destHeight = GetBottom();

	// Calcular la relación de aspecto de la imagen y del destino
	float imageAspectRatio = static_cast<float>(m_textureWidth) / m_textureHeight;
	float destAspectRatio = static_cast<float>(destWidth) / destHeight;

	int drawWidth, drawHeight;

	if (destAspectRatio > imageAspectRatio) {
		// El área de destino es más ancha en proporción a la imagen: ajustar por ancho
		drawWidth = destWidth;
		drawHeight = static_cast<int>(drawWidth / imageAspectRatio);
	} else {
		// El área de destino es más alta en proporción, o igual: ajustar por altura
		drawHeight = destHeight;
		drawWidth = static_cast<int>(imageAspectRatio * drawHeight);
	}

	// Calcular posición inicial para asegurar que la imagen esté centrada
	int left = (destWidth - drawWidth) / 2;
	int top = (destHeight - drawHeight) / 2;
	int right = left + drawWidth;
	int bottom = top + drawHeight;

	// Pintar la textura ajustada y centrada, permitiendo que se extienda fuera del área visible si es necesario
	PaintTexture(left, top, right, bottom);
}

void Bitmap::SetColor(ColorF color) {
	m_color = color;
}

void Bitmap::OnPaint(GL *gl) {
	Load();

	if ((m_textureWidth == 0) || (m_textureHeight == 0))
		return;

	GLboolean oldDepth = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	GLboolean oldBlend = glIsEnabled(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, m_texture);

	glPushMatrix();
	glColor4f(m_color.GetR(), m_color.GetG(), m_color.GetB(), m_color.GetA());

	glBegin(GL_QUADS);

	switch (m_stretchMode) {
		case StretchMode::Enum::NoResize:
			PaintNoResized();
			break;
		case StretchMode::Enum::Tile:
			PaintTiled();
			break;
		case StretchMode::Enum::Stretch:
			PaintStretched();
			break;
		case StretchMode::Enum::AspectFit:
			PaintAspectFit();
			break;
		case StretchMode::Enum::AspectFill:
			PaintAspectFill();
			break;
	}

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glPopMatrix();

	if (!oldBlend)
		glDisable(GL_BLEND);
	if (oldDepth)
		glEnable(GL_DEPTH_TEST);
}
