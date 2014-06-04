#ifndef _AWUI_IO_FILE_H
#define _AWUI_IO_FILE_H

namespace awui {
	class String;

	namespace IO {
		struct FileMode {
			enum Enum {
				Append,
				Create,
				CreateNew,
				Open,
				OpenOrCreate,
				Truncate
			};
		};

		class FileStream;

		class File {
			public:
				static FileStream * Open(const String path, FileMode::Enum mode);
		};
	}
}

#endif