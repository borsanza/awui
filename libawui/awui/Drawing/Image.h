#pragma once

#include <awui/Object.h>
#include <stdint.h>
#include <vector>

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;
typedef unsigned int GLuint;

namespace awui {
	class String;

	namespace Drawing {
		class Image : public Object {
			friend class Graphics;

		  private:
			// Todas las Image vivas (UnloadAll). Nunca se destruye: una Image que muera al salir la encuentra viva
			static std::vector<Image *> &List();
			int m_width;
			int m_height;
			cairo_surface_t *m_cairo_surface;
			cairo_t *m_cr;
			unsigned char *m_image;
			bool m_loaded;
			GLuint m_texture;

			void SyncWithCairo();
			void Create(int width, int height);

		  public:
			Image(int width, int height);
			Image(String name);
			virtual ~Image();

			int GetWidth();
			int GetHeight();

			void Load();
			void Unload();
			static void UnloadAll();
			GLuint GetTexture();

			void SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF);
			void Update();
			void Clear();
		};
	} // namespace Drawing
} // namespace awui
