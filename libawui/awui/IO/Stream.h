#ifndef _AWUI_IO_STREAM_H
#define _AWUI_IO_STREAM_H

#include <stdint.h>

namespace awui {
	namespace IO {
		class Stream {
			public:
				virtual void Close() = 0;

				virtual void SetPosition(int64_t value) = 0;
				virtual int64_t GetPosition() = 0;

				virtual int64_t GetLength() = 0;

				virtual uint8_t ReadByte() = 0;
				virtual void WriteByte(uint8_t value) = 0;
		};
	}
}

#endif