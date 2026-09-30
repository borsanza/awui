#pragma once

#include <assert.h>
#include <stdint.h>

namespace awui::Emulation::Common {
	class Ram {
	  private:
		uint8_t *m_data;
		uint32_t m_size;

	  public:
		Ram(uint32_t size);
		virtual ~Ram();

		void Clear();
		void Resize(uint32_t size);

		inline uint8_t ReadByte(int64_t pos) const { /*assert(pos < m_size);*/
			return m_data[pos];
		}
		inline void WriteByte(int64_t pos, uint8_t value) { /*assert(pos < m_size);*/
			m_data[pos] = value;
		}
	};
} // namespace awui::Emulation::Common
