#ifndef _AWUI_IO_FILESTREAM_H
#define _AWUI_IO_FILESTREAM_H

#include <awui/IO/Stream.h>

namespace awui {
	namespace IO {
		class FileStream : public Stream {
			public:
				virtual ~FileStream();

				virtual void Close();

				virtual void SetPosition(int64_t value);
				virtual int64_t GetPosition();

				virtual int64_t GetLength();
		};
	}
}

#endif