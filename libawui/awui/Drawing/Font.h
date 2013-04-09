// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <string>

namespace awui {
	namespace Drawing {
		class Font {
		private:
			std::string font;
			bool bold;
			float size;

		public:
			Font(const std::string& font, float size);

			const std::string& GetFont();
			bool GetBold();
			float GetSize();
		};
	}
}
