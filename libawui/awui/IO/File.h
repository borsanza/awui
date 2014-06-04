// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIFILE_H__
#define __AWUIFILE_H__

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
				static FileStream * Open(const String path, FileMode mode);
		};
	}
}

#endif
