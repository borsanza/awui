#pragma once

#include <awui/Object.h>

namespace awui {
	class ColorF : public Object {
	  private:
		float m_r, m_g, m_b, m_a;

	  public:
		ColorF();

		virtual bool IsClass(Classes objectClass) const override;
		virtual String ToString() const override;

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

		ColorF &operator=(const ColorF &other);

		bool operator!=(const ColorF &b) const;
	};
} // namespace awui
