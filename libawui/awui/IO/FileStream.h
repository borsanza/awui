#ifndef _AWUI_IO_FILESTREAM_H
#define _AWUI_IO_FILESTREAM_H

#include <awui/IO/Stream.h>
#include <awui/IO/File.h>

#include <stdio.h>

namespace awui {
	class String;

	namespace IO {
		class FileStream : public Stream {
			private:
				FILE * _file;
				long _length;
				long _pos;

			public:
				FileStream(const String path, FileMode::Enum mode, FileAccess::Enum access);
				FileStream(const String path, FileMode::Enum mode);
				virtual ~FileStream();

				virtual void Close();

				virtual uint32_t GetPosition();
				virtual void SetPosition(uint32_t value);

				virtual uint32_t GetLength();

				virtual uint8_t ReadByte();
				virtual void WriteByte(uint8_t value);
		};
	}
}

#endif
