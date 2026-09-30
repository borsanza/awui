#pragma once

#include <stdint.h>

namespace awui::Emulation::Chip8 {
	class Registers {
	  private:
		uint8_t *m_v;
		uint8_t m_length;
		uint32_t m_i;

	  public:
		Registers(uint8_t n);
		virtual ~Registers();

		void Clear();

		void SetV(uint8_t pos, uint8_t value);
		uint8_t GetV(uint8_t pos);

		void SetI(uint32_t value);
		uint32_t GetI();
	};
} // namespace awui::Emulation::Chip8
