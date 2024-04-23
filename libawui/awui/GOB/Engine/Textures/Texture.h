#pragma once

typedef unsigned int GLuint;

#include <awui/String.h>

namespace awui::GOB::Engine {
	class Texture {
		static constexpr int GL_NEAREST = 0x2600;
		static constexpr int GL_LINEAR = 0x2601;

		// TextureMinFilter
		static constexpr int GL_NEAREST_MIPMAP_NEAREST = 0x2700;
		static constexpr int GL_LINEAR_MIPMAP_NEAREST = 0x2701;
		static constexpr int GL_NEAREST_MIPMAP_LINEAR = 0x2702;
		static constexpr int GL_LINEAR_MIPMAP_LINEAR = 0x2703;

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

		void setMinFilter(int filter);
	};
} // namespace awui::GOB::Engine
