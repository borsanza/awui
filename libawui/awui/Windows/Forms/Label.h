#pragma once

#include <awui/Drawing/GlyphMetrics.h>
#include <awui/String.h>
#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Drawing {
		class Graphics;
		class Image;
	}

	namespace OpenGL {
		class GL;
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

					virtual void OnPaint(OpenGL::GL * gl);

					void DrawLines(int x, int y);

					virtual void SetForeColor(const Drawing::Color color);
					virtual void SetFont(const Drawing::Font * font);
			};
		}
	}
}
