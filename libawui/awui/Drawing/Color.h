#pragma once

#include <awui/String.h>
#include <cstdint>

namespace awui::Drawing {
	// Tipo de valor: sin herencia ni métodos virtuales (se copia y se guarda por valor en todas partes)
	class Color {
	  private:
		uint8_t m_r, m_g, m_b, m_a;

		// Para los colores con nombre (constexpr: sin coste al arrancar)
		struct Argb {};
		constexpr Color(Argb, uint8_t a, uint8_t r, uint8_t g, uint8_t b) : m_r(r), m_g(g), m_b(b), m_a(a) {}

	  public:
		// Colores con nombre (los mismos valores que System.Drawing.Color de .NET)
		static const Color Transparent;
		static const Color Black;
		static const Color White;
		static const Color Gray;
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Yellow;

		constexpr Color() : m_r(0), m_g(0), m_b(0), m_a(0) {}
		Color(uint32_t color);
		Color(float r, float g, float b, float a = 1.0f);

		String ToString() const;

		uint8_t GetA() const;
		uint8_t GetR() const;
		uint8_t GetG() const;
		uint8_t GetB() const;
		uint32_t ToArgb() const;
		float GetBrightness() const;
		float GetHue() const;
		float GetSaturation() const;

		static Color FromArgb(uint32_t argb);
		static Color FromArgb(uint8_t alpha, Color baseColor);
		static Color FromArgb(uint8_t red, uint8_t green, uint8_t blue);
		static Color FromArgb(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue);

		bool operator==(const Color &other) const = default;
	};

	inline constexpr Color Color::Transparent = Color(Color::Argb{}, 0, 0, 0, 0);
	inline constexpr Color Color::Black = Color(Color::Argb{}, 255, 0, 0, 0);
	inline constexpr Color Color::White = Color(Color::Argb{}, 255, 255, 255, 255);
	inline constexpr Color Color::Gray = Color(Color::Argb{}, 255, 128, 128, 128);
	inline constexpr Color Color::Red = Color(Color::Argb{}, 255, 255, 0, 0);
	inline constexpr Color Color::Green = Color(Color::Argb{}, 255, 0, 128, 0);
	inline constexpr Color Color::Blue = Color(Color::Argb{}, 255, 0, 0, 255);
	inline constexpr Color Color::Yellow = Color(Color::Argb{}, 255, 255, 255, 0);
} // namespace awui::Drawing
