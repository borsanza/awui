#pragma once

#include <awui/Object.h>

namespace awui::Drawing {
	class Point;

	// Tipo de valor: sin herencia ni métodos virtuales (se copia y se guarda por valor en todas partes)
	class Size {
	  private:
		float m_width;
		float m_height;

	  public:
		Size();
		Size(const Point pt);
		Size(float width, float height);
		~Size();

		float GetWidth() const;
		void SetWidth(float width);

		float GetHeight() const;
		void SetHeight(float height);

		Size &operator=(const Size &other);

		String ToString() const;
	};
} // namespace awui::Drawing
