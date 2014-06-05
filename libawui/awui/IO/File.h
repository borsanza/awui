#ifndef _AWUI_IO_FILE_H
#define _AWUI_IO_FILE_H

namespace awui {
	class String;

	namespace IO {
		struct FileAccess {
			enum Enum {
				Read,
				ReadWrite,
				Write
			};
		};

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
				static FileStream * Open(const String path, FileMode::Enum mode, FileAccess::Enum access);
		};
	}
}

#endif