#pragma once

#include <stdint.h>

namespace awui::IO {
	class Stream {
	  public:
		virtual ~Stream();

		virtual void Close() = 0;

		virtual void SetPosition(uint32_t value) = 0;
		virtual uint32_t GetPosition() = 0;

		virtual uint32_t GetLength() = 0;

		virtual uint8_t ReadByte() = 0;
		virtual void WriteByte(uint8_t value) = 0;
	};
} // namespace awui::IO
