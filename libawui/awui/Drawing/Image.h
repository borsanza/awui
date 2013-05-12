// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;
typedef unsigned int	GLuint;

namespace awui {
	class String;

	namespace Drawing {
		class Image : public Object {
			friend class Graphics;

			private:
				int width;
				int height;
				int btpp;
				cairo_surface_t *cairo_surface;
				cairo_t *cr;
				unsigned char *image;
				bool loaded;
				GLuint texture;

			public:
				Image(int width, int height);
				Image(String name);
				virtual ~Image();

				virtual int IsClass(Classes::Enum objectClass) const;

				int GetWidth();
				int GetHeight();

				void Load();
				GLuint GetTexture();
		};
	}
}
