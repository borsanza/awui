// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;

namespace awui {
	class String;

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
			int GetBorderY(Drawing::Font * font) const;

		public:
			virtual ~Graphics();

			virtual int IsClass(Classes::Enum objectClass) const;

			static Graphics * FromImage(Drawing::Image *image);

			void Clear(const Drawing::Color color);
			void DrawRectangle(Drawing::Pen * pen, float x, float y, float width, float height);
			void FillRectangle(const Drawing::Color color, float x, float y, float width, float height);
			void DrawImage(Drawing::Image * image, float x, float y);
			void DrawImage(Drawing::Image * image, float x, float y, float width, float height);
			void DrawLine(Drawing::Pen * pen, float x1, float y1, float x2, float y2);

			Drawing::Size GetMeasureText(const String text, Drawing::Font *font) const;
			void DrawString(const String text, Drawing::Font * font, const Drawing::Color color, float x, float y);
		};
	}
}
