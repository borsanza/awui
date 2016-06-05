#pragma once

#include <awui/String.h>

namespace awui {

	namespace IO {
		struct FileAccess {
			enum Enum {
				Read = 1,
				Write = 2,
				ReadWrite = 3,
			};
		};

		struct FileMode {
			enum Enum {
				CreateNew = 1,
				Create = 2,
				Open = 3,
				OpenOrCreate = 4,
				Truncate = 5,
				Append = 6,
			};
		};

		class File {
			public:
				static bool Exists(String path);
		};

	}
}
