#pragma once

namespace awui {
	class String;

	namespace Drawing {
		class Point;

		class Size {
			private:
				int width;
				int height;

			public:
				Size();
				Size(const Point pt);
				Size(int width, int height);
				virtual ~Size();

				int GetWidth() const;
				void SetWidth(int width);

				int GetHeight() const;
				void SetHeight(int height);

				Size & operator= (const Size & other);

				virtual String ToString();
		};
	}
}
