// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>
#include <string>

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;

namespace awui {
	namespace Drawing {
		class Color;
		class Font;
		class Image;
		class Pen;
		class Size;

		class Graphics : public Object {
		private:
			cairo_surface_t *cairo_surface;
			cairo_t *cr;

			void SetPen(Drawing::Pen * pen);
			Graphics();

		public:
			~Graphics();

			virtual int IsClass(awui::Classes objectClass) const;

			static Graphics * FromImage(Drawing::Image *image);

			void Clear(const Drawing::Color color);
			void DrawRectangle(Drawing::Pen * pen, float x, float y, float width, float height);
			void FillRectangle(const Drawing::Color color, float x, float y, float width, float height);
			void DrawImage(Drawing::Image * image, float x, float y);
			void DrawImage(Drawing::Image * image, float x, float y, float width, float height);
			void DrawLine(Drawing::Pen * pen, float x1, float y1, float x2, float y2);

			Drawing::Size GetMeasureText(const std::string& text, Drawing::Font *font) const;
			void DrawString(const std::string& text, Drawing::Font * font, float x, float y);
		};
	}
}
