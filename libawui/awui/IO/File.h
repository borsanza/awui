#ifndef _AWUI_IO_FILE_H
#define _AWUI_IO_FILE_H

namespace awui {
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
	}
}

#endif