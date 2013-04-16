// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/String.h>

namespace awui {
	namespace Drawing {
		class Font {
		private:
			String font;
			bool bold;
			float size;

		public:
			Font(const String font, float size);

			const String GetFont() const;
			bool GetBold();
			float GetSize();
		};
	}
}
