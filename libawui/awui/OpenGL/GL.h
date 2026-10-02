#pragma once

#include <awui/Drawing/Rectangle.h>

namespace awui {
	namespace Drawing {
		class Color;
		class Image;
	}

	namespace UI {
		class Control;
		class Form;
	} // namespace UI

	namespace OpenGL {
		class GL {
			friend class awui::UI::Control;
			friend class awui::UI::Form;

		  private:
			awui::Drawing::Rectangle m_clippingBase;
			awui::Drawing::Rectangle m_clipping;

			GL();
			void SetClippingBase(awui::Drawing::Rectangle rectangle);
			awui::Drawing::Rectangle GetClippingBase() const;

		  public:
			void SetClipping(awui::Drawing::Rectangle rectangle);
			awui::Drawing::Rectangle GetClipping() const;

			awui::Drawing::Rectangle GetClippingResult() const;

			void SetClipping();

			// Con el Painter (shaders). Los extremos están incluidos, en píxeles del control
			static void DrawLine(int x, int y, int x2, int y2, const awui::Drawing::Color &color);
			static void DrawRectangle(int x1, int y1, int x2, int y2, const awui::Drawing::Color &color);
			static void FillRectangle(int x1, int y1, int x2, int y2, const awui::Drawing::Color &color);
			static void DrawImageGL(awui::Drawing::Image *image, int x, int y);
			static void DrawImageGL(awui::Drawing::Image *image, int x, int y, int width, int height, float opacity = 1.0f);
		};
	} // namespace OpenGL
} // namespace awui
