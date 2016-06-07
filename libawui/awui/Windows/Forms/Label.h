#pragma once

#include <awui/Drawing/GlyphMetrics.h>
#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Drawing {
		class Graphics;
		class Image;
	}

	namespace Windows {
		namespace Forms {
			class Label : public Control {
				private:
					String text;
					Drawing::ContentAlignment::Enum textAlign;

					Drawing::Image * image;
					Drawing::Graphics *g;
					Drawing::GlyphMetrics metrics;

					void UpdateBufferText();

				public:
					Label();
					virtual ~Label();

					virtual int IsClass(Classes::Enum objectClass) const;

					const String GetText();
					void SetText(const String str);

					const Drawing::ContentAlignment::Enum GetTextAlign();
					void SetTextAlign(Drawing::ContentAlignment::Enum textAlign);

					void Draw(int x, int y, int width, int height);
					virtual void OnPaint(OpenGL::GL * gl);

					void DrawLines(int x, int y);

					virtual void SetForeColor(const Drawing::Color color);
					virtual void SetFont(const Drawing::Font font);

					int GetLabelWidth() const;
			};
		}
	}
}
