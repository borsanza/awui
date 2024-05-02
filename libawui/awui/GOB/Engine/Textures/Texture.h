#pragma once

#include <awui/GOB/Engine/Shaders/Shader.h>
#include <awui/String.h>

typedef unsigned int GLuint;

namespace awui::GOB::Engine {
	class Shader;

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

		void Load();
		void Unload();
		void UpdateTextureFilters();

		Shader *m_shader;

	  public:
		static constexpr int TEXTURE_NEAREST = 0x2600;
		static constexpr int TEXTURE_LINEAR = 0x2601;
		static constexpr int TEXTURE_NEAREST_MIPMAP_NEAREST = 0x2700;
		static constexpr int TEXTURE_LINEAR_MIPMAP_NEAREST = 0x2701;
		static constexpr int TEXTURE_NEAREST_MIPMAP_LINEAR = 0x2702;
		static constexpr int TEXTURE_LINEAR_MIPMAP_LINEAR = 0x2703;

		Texture(const String file, int minFilter = TEXTURE_LINEAR, int magFilter = TEXTURE_LINEAR);
		virtual ~Texture();

		void SetMinFilter(int filter);
		void SetMagFilter(int filter);

		void BindTexture();
		void UnBindTexture();

		GLuint GetProgram() { return m_shader->GetProgram(); }
	};
} // namespace awui::GOB::Engine
