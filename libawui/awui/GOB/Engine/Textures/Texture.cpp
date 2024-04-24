#include "Texture.h"

#include <SDL_image.h>
#include <SDL_opengl.h>
#include <awui/Console.h>

using namespace awui::GOB::Engine;

Texture::Texture(const String file, int minFilter, int magFilter) : m_file(file), m_minFilter(minFilter), m_magFilter(magFilter) {
	m_textureWidth = 0;
	m_textureHeight = 0;

	m_loaded = false;
	m_texture = -1;
	m_needUpdateFilters = true;
	m_errorOnLoad = false;

	m_oldBlend = false;
	m_oldDepth = true;
	m_oldTexture = false;
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
		m_texture = -1;
		return;
	}

	glBindTexture(GL_TEXTURE_2D, m_texture);
	if (glGetError() != GL_NO_ERROR) {
		Console::Error->WriteLine("OpenGL error: Failed to bind texture.");
		m_errorOnLoad = true;
		glDeleteTextures(1, &m_texture);
		SDL_FreeSurface(textureImage);
		m_texture = -1;
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLenum internalFormat = textureImage->format->BytesPerPixel == 4 ? GL_RGBA8 : GL_RGB8;
	GLenum textureFormat = textureImage->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, textureImage->w, textureImage->h, 0, textureFormat, GL_UNSIGNED_BYTE, textureImage->pixels);
	if (glGetError() != GL_NO_ERROR) {
		Console::Error->WriteLine("OpenGL error: Failed to load texture image.");
		m_errorOnLoad = true;
		glDeleteTextures(1, &m_texture);
		SDL_FreeSurface(textureImage);
		m_texture = -1;
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

	if (m_texture != 0) {
		glDeleteTextures(1, &m_texture);
		m_texture = -1;
	}

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

void Texture::BindTexture() {
	Load();

	if ((m_textureWidth == 0) || (m_textureHeight == 0)) {
		return;
	}

	/*
		m_oldTexture = glIsEnabled(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_2D);
		m_oldDepth = glIsEnabled(GL_DEPTH_TEST);
		glDisable(GL_DEPTH_TEST);
		m_oldBlend = glIsEnabled(GL_BLEND);
		glEnable(GL_BLEND);
	*/
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	UpdateTextureFilters();
}

void Texture::UnBindTexture() {
	/*
		if (!m_oldBlend)
			glDisable(GL_BLEND);
		if (m_oldDepth)
			glEnable(GL_DEPTH_TEST);
		if (!m_oldTexture)
			glDisable(GL_TEXTURE_2D);
	*/
}

void Texture::UpdateTextureFilters() {
	if (!m_needUpdateFilters) {
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);

	m_needUpdateFilters = false;
}
