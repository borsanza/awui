#pragma once

#include <awui/String.h>

typedef unsigned int GLuint;

namespace awui::GOB::Engine {
	class Texture {
	  private:
		GLuint m_texture;
		String m_file;
		bool m_loaded;
		int m_textureWidth;
		int m_textureHeight;

		int m_minFilter;
		int m_magFilter;
		bool m_needUpdateFilters;
		bool m_errorOnLoad;
		bool m_oldBlend;
		bool m_oldDepth;
		bool m_oldTexture;

		void Load();
		void Unload();
		void UpdateTextureFilters();

	  public:
		static constexpr int TEXTURE_NEAREST = 0x2600;
		static constexpr int TEXTURE_LINEAR = 0x2601;

		Texture(const String file, int minFilter = TEXTURE_LINEAR, int magFilter = TEXTURE_LINEAR);
		virtual ~Texture();

		void SetMinFilter(int filter);
		void SetMagFilter(int filter);

		void BindTexture();
		void UnBindTexture();
	};
} // namespace awui::GOB::Engine
