#pragma once

namespace awui {
	namespace Drawing {
		struct ContentAlignment {
			enum Enum {
				TopLeft = 1,
				TopCenter = 2,
				TopRight = 4,
				MiddleLeft = 16,
				MiddleCenter = 32,
				MiddleRight = 64,
				BottomLeft = 256,
				BottomCenter = 512,
				BottomRight = 1024,
			};
		};

		class GlyphMetrics {
			private:
				int width;
				int height;
				int advancex;
				int advancey;
				int bearingx;
				int bearingy;
				int ascent;
				int descent;

			public:
				GlyphMetrics();

				int GetWidth() const;
				int GetHeight() const;
				void SetWidth(int width);
				void SetHeight(int height);

				int GetAdvanceX() const;
				int GetAdvanceY() const;
				void SetAdvanceX(int width);
				void SetAdvanceY(int height);

				int GetBearingX() const;
				int GetBearingY() const;
				void SetBearingX(int width);
				void SetBearingY(int height);

				int GetAscent() const;
				int GetDescent() const;
				void SetAscent(int ascent);
				void SetDescent(int descent);
		};
	}
}
