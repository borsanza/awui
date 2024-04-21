#pragma once

#include "stdint.h"

namespace awui::GOB::Engine {
	class Color {
	  private:
		union {
			struct {
				uint8_t m_r, m_g, m_b, m_a;
			};
			struct {
				uint32_t m_color32;
			};
			uint8_t m_color[4];
		};

	  public:
		Color(uint32_t color);
		Color(float r, float g, float b, float a = 1.0f);

		uint8_t GetR() const { return m_r; }
		uint8_t GetG() const { return m_g; }
		uint8_t GetB() const { return m_b; }
		uint8_t GetA() const { return m_a; }
	};
} // namespace awui::GOB::Engine
