#pragma once

#include <awui/Object.h>
#include <awui/String.h>

namespace awui::Drawing {
	struct FontStyle {
		enum Enum {
			Regular = 0,
			Bold = 1,
			Italic = 2,
			Underline = 4,
			Strikeout = 8,
		};
	};

	class Font : public Object {
	  private:
		String m_font;
		float m_size;
		int m_style;

	  public:
		Font(const String font, float size);
		Font(const String font, float size, int style);

		const String GetFont() const;
		bool GetBold() const;
		bool GetItalic() const;
		bool GetUnderline() const;
		bool GetStrikeout() const;
		float GetSize() const;
		virtual String ToString() const override;

		Font &operator=(const Font &other);
	};
} // namespace awui::Drawing
