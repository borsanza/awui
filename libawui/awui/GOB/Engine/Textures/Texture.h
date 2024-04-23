#pragma once

typedef unsigned int GLuint;

#include <awui/String.h>

namespace awui::GOB::Engine {
	class Texture {
	  private:
		String m_file;
		bool m_loaded;
		int m_textureWidth;
		int m_textureHeight;
		GLuint m_texture;

		void Load();
		void Unload();

	  public:
		Texture(const String file);
		virtual ~Texture();
	};
} // namespace awui::GOB::Engine
