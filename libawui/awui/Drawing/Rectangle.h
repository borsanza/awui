#pragma once

#include <awui/Drawing/Point.h>
#include <awui/Drawing/Size.h>

namespace awui {
	class String;
}

namespace awui::Drawing {
	// Tipo de valor: sin herencia ni métodos virtuales (se copia y se guarda por valor en todas partes)
	class Rectangle {
	  private:
		Point m_location;
		Size m_size;

	  public:
		Rectangle();
		Rectangle(const Point &location, const Size &size);
		Rectangle(float x, float y, float width, float height);

		inline float GetWidth() const { return m_size.GetWidth(); }
		inline float GetHeight() const { return m_size.GetHeight(); }
		inline float GetX() const { return m_location.GetX(); }
		inline float GetY() const { return m_location.GetY(); }

		void SetWidth(float width);
		void SetHeight(float height);
		void SetX(float x);
		void SetY(float y);
		void SetLocation(const Point &location);
		void SetSize(const Size &size);

		float GetBottom() const;
		inline float GetLeft() const { return m_location.GetX(); }
		float GetRight() const;
		inline float GetTop() const { return m_location.GetY(); }
		Point GetLocation() const;
		Size GetSize() const;
		void SetLocation(float x, float y) { SetLocation(Point(x, y)); }
		void SetSize(float width, float height);
		void Inflate(const Size &size);
		void Inflate(float width, float height);
		void Offset(const Point &pos);
		void Offset(float x, float y);

		static Rectangle FromLTRB(float left, float top, float right, float bottom);
		static Rectangle Intersect(const Rectangle &rectangle1, const Rectangle &rectangle2);
		void Intersect(const Rectangle &rectangle);

		bool operator==(const Rectangle &other) const = default;

		// Sin área (ancho o alto nulos)
		bool IsEmpty() const;
		// El punto está dentro (de X a X + ancho sin incluir, como los píxeles que ocupa)
		bool Contains(float x, float y) const;
		bool Contains(const Point &point) const;
		// El rectángulo más pequeño que contiene a los dos (uno vacío no cuenta)
		static Rectangle Union(const Rectangle &rectangle1, const Rectangle &rectangle2);

		String ToString() const;
	};
} // namespace awui::Drawing
