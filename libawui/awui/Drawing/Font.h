// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIFONT_H__
#define __AWUIFONT_H__

#include <awui/Object.h>
#include <awui/String.h>

namespace awui {
	namespace Drawing {
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
				String font;
				float size;
				int style;

			public:
				Font(const String font, float size);
				Font(const String font, float size, int style);

				const String GetFont() const;
				bool GetBold();
				bool GetItalic();
				bool GetUnderline();
				bool GetStrikeout();
				float GetSize();
				virtual String ToString();

				Font & operator=(const Font & other);
		};
	}
}

#endif
