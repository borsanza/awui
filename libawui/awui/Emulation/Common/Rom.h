#pragma once

#include <awui/IO/MemoryStream.h>
#include <awui/String.h>

namespace awui::Emulation::Common {
	class Rom {
	  private:
		IO::MemoryStream *_rom;
		String _file;
		uint8_t _numPages;

	  public:
		Rom(int32_t capacity);
		virtual ~Rom();

		void LoadRom(const String file);

		inline uint8_t ReadByte(uint32_t pos) const { return this->_rom->ReadByte(pos); }
		inline void WriteByte(uint32_t pos, uint8_t value) { this->_rom->WriteByte(pos, value); }

		void Reload();
		uint32_t GetCRC32() const;
		uint8_t GetNumPages() const { return this->_numPages; }
	};
} // namespace awui::Emulation::Common
