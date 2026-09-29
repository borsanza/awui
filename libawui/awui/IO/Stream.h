#pragma once

#include <stdint.h>

namespace awui::IO {
	class Stream {
	  public:
		virtual ~Stream();

		virtual void Close() = 0;

		virtual void SetPosition(uint32_t value) = 0;
		virtual uint32_t GetPosition() const = 0;

		virtual uint32_t GetLength() const = 0;

		virtual uint8_t ReadByte() = 0;
		virtual void WriteByte(uint8_t value) = 0;

		// Por bloques. La versión base va byte a byte; FileStream y MemoryStream tienen la suya.
		// Read devuelve cuántos bytes ha leído de verdad (menos al llegar al final)
		virtual uint32_t Read(uint8_t *buffer, uint32_t count);
		virtual void Write(const uint8_t *buffer, uint32_t count);
	};
} // namespace awui::IO
