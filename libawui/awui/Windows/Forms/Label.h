#pragma once

#include <awui/Drawing/GlyphMetrics.h>
#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Drawing {
		class Graphics;
		class Image;
	} // namespace Drawing

	namespace Windows::Forms {
		class Label : public Control {
		  private:
			String m_text;
			Drawing::ContentAlignment::Enum m_textAlign;

			Drawing::Image *m_image;
			Drawing::Graphics *m_g;
			Drawing::GlyphMetrics m_metrics;
			float m_scrolled;

			void UpdateBufferText();

		  public:
			Label();
			virtual ~Label();

			virtual bool IsClass(Classes objectClass) const override;

			const String GetText() const;
			void SetText(const String str);

			const Drawing::ContentAlignment::Enum GetTextAlign();
			void SetTextAlign(Drawing::ContentAlignment::Enum textAlign);

			void Draw(int x, int y, int width, int height);
			virtual void OnPaint(OpenGL::GL *gl);

			virtual void SetForeColor(const Color color);
			virtual void SetFont(const Drawing::Font font);

			void SetScrolled(float scroll);
			float GetScrolled() const;

			int GetLabelWidth() const;
		};
	} // namespace Windows::Forms
} // namespace awui
