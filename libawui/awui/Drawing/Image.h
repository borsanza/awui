// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;

namespace awui {
	namespace OpenGL {
		class GL;
	}

	class String;

	namespace Drawing {
		class Image : public Object {
			friend class Graphics;
			friend class awui::OpenGL::GL;

		protected:
			int width;
			int height;
			int btpp;
			cairo_surface_t *cairo_surface;
			cairo_t *cr;
			unsigned char *image;

		public:
			Image(int width, int height);
			Image(String name);
			virtual ~Image();

			virtual int IsClass(Classes::Enum objectClass) const;

			int GetWidth();
			int GetHeight();
		};
	}
}
