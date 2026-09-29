#pragma once

#include <awui/IO/MemoryStream.h>
#include <awui/String.h>

namespace awui::Emulation::Common {
	class Rom {
	  private:
		IO::MemoryStream *_rom;
		String _file;
		uint32_t _size; // Tamaño del fichero
		uint32_t _mask; // Tamaño redondeado a la siguiente potencia de 2, menos 1

		void UpdateSize();

	  public:
		Rom(int32_t capacity);
		virtual ~Rom();

		void LoadRom(const String file);

		// Como en el hardware, las líneas de dirección que sobran se ignoran: la ROM se repite cada potencia de 2.
		// Si el tamaño no es potencia de 2, el hueco hasta la siguiente devuelve 0xFF (bus abierto)
		inline uint8_t ReadByte(uint32_t pos) const {
			pos &= this->_mask;
			return (pos < this->_size) ? this->_rom->ReadByte(pos) : 0xFF;
		}
		inline void WriteByte(uint32_t pos, uint8_t value) { this->_rom->WriteByte(pos, value); }

		// Quita los primeros bytes de la ROM (p. ej. la cabecera de 512 bytes de las copiadoras)
		void RemoveHeader(uint32_t bytes);

		void Reload();
		uint32_t GetCRC32() const;
		inline uint32_t GetSize() const { return this->_size; }
	};
} // namespace awui::Emulation::Common
