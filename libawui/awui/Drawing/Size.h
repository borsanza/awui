#pragma once

#include <awui/Object.h>

namespace awui {
	class String;

	namespace Drawing {
		class Point;

		class Size : public Object {
			private:
				float m_width;
				float m_height;

			public:
				Size();
				Size(const Point pt);
				Size(float width, float height);
				virtual ~Size();

				float GetWidth() const;
				void SetWidth(float width);

				float GetHeight() const;
				void SetHeight(float height);

				Size & operator= (const Size & other);

				virtual String ToString() const override;
		};
	}
}
