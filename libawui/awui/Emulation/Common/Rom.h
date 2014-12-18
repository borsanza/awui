#ifndef _AWUI_EMULATION_COMMON_ROM_H
#define _AWUI_EMULATION_COMMON_ROM_H

#include <awui/String.h>
#include <awui/IO/MemoryStream.h>

namespace awui {
	namespace Emulation {
		namespace Common {
			class Rom {
				private:
					IO::MemoryStream * _rom;
					String _file;
					uint8_t _numPages;

				public:
					Rom(int32_t capacity);
					~Rom();

					void LoadRom(const String file);

					inline uint8_t ReadByte(uint32_t pos) const { return this->_rom->ReadByte(pos); }
					inline void WriteByte(uint32_t pos, uint8_t value) {	this->_rom->WriteByte(pos, value); }

					void Reload();
					uint32_t GetCRC32() const;
					uint8_t GetNumPages() const { return this->_numPages; }
			};
		}
	}
}

#endif
