#include "Texture.h"

#include <cmath>

#include <SDL_image.h>
#include <SDL_opengl.h>
#include <awui/Console.h>

using namespace awui::GOB::Engine;

Texture::Texture(const String file, int minFilter, int magFilter) : m_file(file), m_minFilter(minFilter), m_magFilter(magFilter) {
	m_textureWidth = 0;
	m_textureHeight = 0;

	m_loaded = false;
	m_texture = 0;
	m_needUpdateFilters = true;
	m_errorOnLoad = false;
	m_wrap = WRAP_CLAMP;
	m_encodeSRGB = false;
}

void Texture::SetWrap(int wrap) {
	m_wrap = wrap;
}

void Texture::SetEncodeSRGB(bool encode) {
	m_encodeSRGB = encode;
}

Texture::~Texture() {
	Unload();
}

void Texture::Load() {
	if (m_errorOnLoad || m_loaded || m_file.IsEmpty()) {
		return;
	}

	SDL_Surface *textureImage = IMG_Load(m_file.ToCharArray());
	if (!textureImage) {
		Console::Error->WriteLine("Failed to load texture: %s", m_file.ToCharArray());
		m_errorOnLoad = true;
		return;
	}

	SDL_Surface *optimizedImage = SDL_ConvertSurfaceFormat(textureImage, SDL_PIXELFORMAT_RGBA32, 0);
	SDL_FreeSurface(textureImage);
	if (!optimizedImage) {
		Console::Error->WriteLine("Failed to optimize texture format: %s", m_file.ToCharArray());
		m_errorOnLoad = true;
		return;
	}
	textureImage = optimizedImage;

	glGenTextures(1, &m_texture);
	if (glGetError() != GL_NO_ERROR) {
		Console::Error->WriteLine("OpenGL error: Failed to generate texture.");
		m_errorOnLoad = true;
		SDL_FreeSurface(textureImage);
		m_texture = 0;
		return;
	}

	glBindTexture(GL_TEXTURE_2D, m_texture);
	if (glGetError() != GL_NO_ERROR) {
		Console::Error->WriteLine("OpenGL error: Failed to bind texture.");
		m_errorOnLoad = true;
		glDeleteTextures(1, &m_texture);
		SDL_FreeSurface(textureImage);
		m_texture = 0;
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap);

	if (m_encodeSRGB) {
		// Lineal a sRGB, con una tabla (el alfa no se toca)
		uint8_t table[256];
		for (int i = 0; i < 256; i++) {
			float linear = i / 255.0f;
			float encoded = (linear <= 0.0031308f) ? (linear * 12.92f) : (1.055f * powf(linear, 1.0f / 2.4f) - 0.055f);
			table[i] = (uint8_t) lroundf(encoded * 255.0f);
		}

		for (int y = 0; y < textureImage->h; y++) {
			uint8_t *row = (uint8_t *) textureImage->pixels + (y * textureImage->pitch);
			for (int x = 0; x < textureImage->w; x++) {
				row[x * 4] = table[row[x * 4]];
				row[x * 4 + 1] = table[row[x * 4 + 1]];
				row[x * 4 + 2] = table[row[x * 4 + 2]];
			}
		}
	}

	GLenum internalFormat = textureImage->format->BytesPerPixel == 4 ? GL_RGBA8 : GL_RGB8;
	GLenum textureFormat = textureImage->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, textureImage->w, textureImage->h, 0, textureFormat, GL_UNSIGNED_BYTE, textureImage->pixels);
	if (glGetError() != GL_NO_ERROR) {
		Console::Error->WriteLine("OpenGL error: Failed to load texture image.");
		m_errorOnLoad = true;
		glDeleteTextures(1, &m_texture);
		SDL_FreeSurface(textureImage);
		m_texture = 0;
		return;
	}

	m_textureWidth = textureImage->w;
	m_textureHeight = textureImage->h;

	SDL_FreeSurface(textureImage);

	m_loaded = true;
}

void Texture::Unload() {
	if (!m_loaded) {
		return;
	}

	glDeleteTextures(1, &m_texture);
	m_texture = 0;

	m_loaded = false;
}

void Texture::SetMinFilter(int filter) {
	if (filter == m_minFilter) {
		return;
	}

	m_minFilter = filter;
	m_needUpdateFilters = true;
}

void Texture::SetMagFilter(int filter) {
	if (filter == m_magFilter) {
		return;
	}

	m_magFilter = filter;
	m_needUpdateFilters = true;
}

GLuint Texture::GetTexture() {
	Load();

	if (!m_loaded || (m_textureWidth == 0) || (m_textureHeight == 0)) {
		return 0;
	}

	glBindTexture(GL_TEXTURE_2D, m_texture);
	UpdateTextureFilters();

	return m_texture;
}

void Texture::UpdateTextureFilters() {
	if (!m_needUpdateFilters) {
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);

	m_needUpdateFilters = false;
}
