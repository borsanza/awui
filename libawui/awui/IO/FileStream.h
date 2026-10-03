#pragma once

#include <awui/IO/File.h>
#include <awui/IO/Stream.h>

#include <stdio.h>

namespace awui {
	class String;

	namespace IO {
		class FileStream : Stream {
		  private:
			FILE *m_file;
			long m_length;
			long m_pos;

		  public:
			FileStream(const String path, FileMode::Enum mode, FileAccess::Enum access);
			FileStream(const String path, FileMode::Enum mode);
			virtual ~FileStream();

			// No se puede copiar: la copia liberaría otra vez el fichero abierto
			FileStream(const FileStream &) = delete;
			FileStream &operator=(const FileStream &) = delete;

			bool IsOpen() const;
			virtual void Close();

			virtual uint32_t GetPosition() const override;
			virtual void SetPosition(uint32_t value);

			virtual uint32_t GetLength() const override;

			virtual uint8_t ReadByte();
			virtual void WriteByte(uint8_t value);
			virtual uint32_t Read(uint8_t *buffer, uint32_t count) override;
			virtual void Write(const uint8_t *buffer, uint32_t count) override;
		};
	} // namespace IO
} // namespace awui
