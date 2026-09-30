#pragma once

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

	// Tipo de valor: sin herencia ni métodos virtuales (se copia y se guarda por valor en todas partes)
	class Font {
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
		String ToString() const;

		bool operator==(const Font &other) const = default;
	};
} // namespace awui::Drawing
