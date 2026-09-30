#pragma once

#include <awui/String.h>

namespace awui::Drawing {
	// Tipo de valor: sin herencia ni métodos virtuales (se copia y se guarda por valor en todas partes)
	class ColorF {
	  private:
		float m_r, m_g, m_b, m_a;

	  public:
		ColorF();

		String ToString() const;

		float GetA() const;
		float GetR() const;
		float GetG() const;
		float GetB() const;
		int ToArgb() const;
		float GetBrightness() const;
		float GetHue() const;
		float GetSaturation() const;

		static ColorF FromArgb(int argb);
		static ColorF FromArgb(float alpha, ColorF baseColor);
		static ColorF FromArgb(float red, float green, float blue);
		static ColorF FromArgb(float alpha, float red, float green, float blue);

		bool operator==(const ColorF &other) const = default;
	};
} // namespace awui::Drawing
