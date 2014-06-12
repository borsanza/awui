#ifndef _AWUI_IO_MEMORYSTREAM_H
#define _AWUI_IO_MEMORYSTREAM_H

#include <awui/IO/Stream.h>

namespace awui {
	namespace IO {
		class MemoryStream : Stream {
			private:
				uint8_t * _data;
				int32_t _capacity;
				int64_t _length;
				int64_t _position;

			public:
				MemoryStream(int32_t capacity);
				virtual ~MemoryStream();

				virtual void Close();

				virtual int64_t GetPosition();
				virtual void SetPosition(int64_t value);

				virtual int64_t GetLength();
				void SetLength(int64_t value);

				int32_t GetCapacity();
				void SetCapacity(int32_t value);

				virtual uint8_t ReadByte();
				virtual void WriteByte(uint8_t value);

				void Clear();
		};
	}
}

#endif