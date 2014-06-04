#ifndef _AWUI_IO_BINARYREADER_H
#define _AWUI_IO_BINARYREADER_H

namespace awui {
	namespace IO {
		class Stream;

		class BinaryReader {
			public:
				BinaryReader(Stream * input);
				virtual ~BinaryReader();

				virtual void Close();
				virtual unsigned char ReadByte();
		};
	}
}

#endif