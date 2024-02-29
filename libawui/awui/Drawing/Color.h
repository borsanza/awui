#pragma once

#include <awui/Object.h>
#include <cstdint>

namespace awui::Drawing {
	class Color : public Object {
		private:
			uint8_t m_a, m_r, m_g, m_b;

		public:
			Color();

			virtual bool IsClass(Classes objectClass) const override;
			virtual String ToString() const override;

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

			Color & operator= (const Color & other);

			bool operator !=(const Color &b) const;
	};
}
