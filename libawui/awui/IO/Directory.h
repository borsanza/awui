#ifndef _AWUI_IO_DIRECTORY_H
#define _AWUI_IO_DIRECTORY_H

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

#endif
