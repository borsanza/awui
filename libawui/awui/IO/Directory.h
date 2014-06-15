#ifndef _AWUI_IO_DIRECTORY_H
#define _AWUI_IO_DIRECTORY_H

#include <awui/String.h>

namespace awui {
	namespace IO {
		class Directory {
			public:
				static String GetCurrentDirectory();
		};
	}
}

#endif