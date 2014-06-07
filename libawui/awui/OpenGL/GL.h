// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Drawing/Rectangle.h>

namespace awui {
	namespace Drawing {
		class Image;
	}
	namespace Windows {
		namespace Forms {
			class Control;
			class Form;
		}
	}

	namespace OpenGL {
		class GL {
			friend class awui::Windows::Forms::Control;
			friend class awui::Windows::Forms::Form;

			private:
				awui::Drawing::Rectangle rectangle1;
				awui::Drawing::Rectangle rectangle2;

				GL();
				void SetClippingBase(awui::Drawing::Rectangle rectangle);
				awui::Drawing::Rectangle GetClippingBase();

			public:
				void SetClipping(awui::Drawing::Rectangle rectangle);
				awui::Drawing::Rectangle GetClipping();

				awui::Drawing::Rectangle GetClippingResult();

				void SetClipping();

				static void DrawLine(int x, int y, int x2, int y2);
				static void DrawRectangle(int x1, int y1, int x2, int y2);
				static void FillRectangle(int x1, int y1, int x2, int y2);
				static void DrawImageGL(awui::Drawing::Image * image, int x, int y);
				static void DrawImageGL(awui::Drawing::Image * image, int x, int y, int width, int height);
		};
	}
}
