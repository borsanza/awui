#ifndef _AWUI_EMULATION_MASTERSYSTEM_RAM_H
#define _AWUI_EMULATION_MASTERSYSTEM_RAM_H

#include <stdint.h>
#include <assert.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Ram {
				private:
					uint8_t * _data;
					uint32_t _size;

				public:
					Ram(uint32_t size);
					~Ram();

					void Clear();
					void Resize(uint32_t size);

					inline uint8_t ReadByte(int64_t pos) const { /*assert(pos < this->_size);*/ return this->_data[pos]; }
					inline void WriteByte(int64_t pos, uint8_t value) { /*assert(pos < this->_size);*/ this->_data[pos] = value; }
			};
		}
	}
}

#endif
