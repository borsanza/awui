#pragma once

#include <assert.h>
#include <stdint.h>

namespace awui::Emulation::Common {
	class Ram {
	  private:
		uint8_t *_data;
		uint32_t _size;

	  public:
		Ram(uint32_t size);
		virtual ~Ram();

		void Clear();
		void Resize(uint32_t size);

		inline uint8_t ReadByte(int64_t pos) const { /*assert(pos < this->_size);*/
			return this->_data[pos];
		}
		inline void WriteByte(int64_t pos, uint8_t value) { /*assert(pos < this->_size);*/
			this->_data[pos] = value;
		}
	};
} // namespace awui::Emulation::Common
