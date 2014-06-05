#ifndef _AWUI_IO_BINARYREADER_H
#define _AWUI_IO_BINARYREADER_H

#include <stdint.h>

namespace awui {
	namespace IO {
		class Stream;

		class BinaryReader {
			private:
				Stream * _stream;

			public:
				BinaryReader(Stream * input);
				virtual ~BinaryReader();

				virtual void Close();
				virtual uint8_t ReadByte();
		};
	}
}

#endif