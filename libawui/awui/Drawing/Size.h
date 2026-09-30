#pragma once

namespace awui {
	class String;
}

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

		float GetWidth() const;
		void SetWidth(float width);

		float GetHeight() const;
		void SetHeight(float height);

		bool operator==(const Size &other) const = default;

		String ToString() const;
	};
} // namespace awui::Drawing
