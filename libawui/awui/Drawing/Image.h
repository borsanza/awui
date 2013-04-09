// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;

namespace awui {
	namespace Drawing {
		class Image : public Object {
			friend class Graphics;

		protected:
			int width;
			int height;
			int btpp;
			cairo_surface_t *cairo_surface;
			cairo_t *cr;

		public:
			// TODO: Cambiar *image a protegido en cuanto pueda
			unsigned char *image;

		public:
			Image(int width, int height);
			Image(const char * name);
			~Image();

			virtual int IsClass(awui::Classes objectClass) const;

			int GetWidth();
			int GetHeight();
		};
	}
}
