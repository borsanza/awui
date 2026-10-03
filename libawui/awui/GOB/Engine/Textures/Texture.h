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
		int m_wrap;
		bool m_encodeSRGB;

		void Load();
		void Unload();
		void UpdateTextureFilters();

	  public:
		static constexpr int TEXTURE_NEAREST = 0x2600;
		static constexpr int TEXTURE_LINEAR = 0x2601;

		static constexpr int WRAP_CLAMP = 0x812F;  // Los bordes se estiran
		static constexpr int WRAP_REPEAT = 0x2901; // Se repite: coordenadas de textura de más de 1

		Texture(const String file, int minFilter = TEXTURE_LINEAR, int magFilter = TEXTURE_LINEAR);
		virtual ~Texture();

		void SetMinFilter(int filter);
		void SetMagFilter(int filter);

		// Antes de usarla por primera vez (se aplican al cargarla):
		void SetWrap(int wrap);
		// Pasa los colores de lineal a sRGB al cargarla. three.js trata como lineales las texturas a las que no se
		// les dice su espacio de color, y al final codifica la imagen en sRGB: salen más claras. Con esto se ven igual
		void SetEncodeSRGB(bool encode);

		// La textura de OpenGL, cargándola la primera vez (y dejándola enlazada), o 0 si no se puede cargar
		GLuint GetTexture();
	};
} // namespace awui::GOB::Engine
