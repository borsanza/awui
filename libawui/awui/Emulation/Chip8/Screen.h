#pragma once

#include <stdint.h>

namespace awui::Emulation::Chip8 {
	class Screen {
	  private:
		uint16_t m_width;
		uint16_t m_height;
		uint32_t *m_data;

	  public:
		Screen(uint16_t width, uint16_t height);
		virtual ~Screen();

		// No se puede copiar: la copia liberaría otra vez los píxeles
		Screen(const Screen &) = delete;
		Screen &operator=(const Screen &) = delete;

		void Clear();
		// Copia el contenido de otra pantalla del mismo tamaño
		void CopyFrom(const Screen &other);

		bool SetPixelXOR(uint16_t x, uint16_t y, bool value);
		void SetPixel(uint16_t x, uint16_t y, uint32_t value);
		uint32_t GetPixel(uint16_t x, uint16_t y);

		uint16_t GetWidth() const;
		inline uint32_t *GetData() { return m_data; } // m_width * m_height píxeles, por filas
		uint16_t GetHeight() const;

		void ScrollLeft(uint8_t columns);
		void ScrollRight(uint8_t columns);
		void ScrollUp(uint8_t lines);
		void ScrollDown(uint8_t lines);
	};
} // namespace awui::Emulation::Chip8
