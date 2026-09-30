#pragma once

#include <awui/IO/Stream.h>

namespace awui::IO {
	class MemoryStream : Stream {
	  private:
		uint8_t *m_data;
		uint32_t m_capacity;
		uint32_t m_length;
		uint32_t m_position;

	  public:
		MemoryStream(uint32_t capacity);
		virtual ~MemoryStream();

		virtual void Close();

		virtual uint32_t GetPosition() const override;
		virtual void SetPosition(uint32_t value);

		virtual uint32_t GetLength() const override;
		void SetLength(uint32_t value);

		uint32_t GetCapacity();
		// Conserva el contenido que quepa; lo nuevo queda a cero
		void SetCapacity(uint32_t value);

		// Fuera de la capacidad se lee 0 y las escrituras se ignoran
		virtual uint8_t ReadByte();
		virtual void WriteByte(uint8_t value);
		virtual uint32_t Read(uint8_t *buffer, uint32_t count) override;
		virtual void Write(const uint8_t *buffer, uint32_t count) override;

		inline uint8_t ReadByte(uint32_t pos) const { return (pos < m_capacity) ? m_data[pos] : 0; }
		inline void WriteByte(uint32_t pos, uint8_t value) {
			if (pos < m_capacity)
				m_data[pos] = value;
		}

		uint32_t GetCRC32() const;

		void Clear();
	};
} // namespace awui::IO
