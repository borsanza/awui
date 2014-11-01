#ifndef _AWUI_IO_MEMORYSTREAM_H
#define _AWUI_IO_MEMORYSTREAM_H

#include <awui/IO/Stream.h>

namespace awui {
	namespace IO {
		class MemoryStream : Stream {
			private:
				uint8_t * _data;
				uint32_t _capacity;
				uint32_t _length;
				uint32_t _position;

			public:
				MemoryStream(uint32_t capacity);
				virtual ~MemoryStream();

				virtual void Close();

				virtual uint32_t GetPosition();
				virtual void SetPosition(uint32_t value);

				virtual uint32_t GetLength();
				void SetLength(uint32_t value);

				uint32_t GetCapacity();
				void SetCapacity(uint32_t value);

				virtual uint8_t ReadByte();
				virtual void WriteByte(uint8_t value);

				inline uint8_t ReadByte(uint32_t pos) const { return this->_data[pos]; }
				inline void WriteByte(uint32_t pos, uint8_t value) { this->_data[pos] = value; }

				uint32_t GetCRC32() const;

				void Clear();
		};
	}
}

#endif
