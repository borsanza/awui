#pragma once

#include <awui/String.h>

namespace awui {
	namespace IO {
		class Directory {
			public:
				static const String GetCurrentDirectory();
				static const String GetWorkingDirectory();
				static const ArrayList * GetFiles(const String path);
		};
	}
}
