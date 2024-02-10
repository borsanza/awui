#pragma once

namespace awui {
	class String;

	namespace IO {

#ifdef _WIN32
		constexpr char DirectorySeparatorChar = '\\';
#else
		constexpr char DirectorySeparatorChar = '/';
#endif

		class Path {
			public:
				static String Combine(const String &path1, const String &path2);
				static String Combine(const String &path1, const String &path2, const String &path3);
				static String Combine(const String &path1, const String &path2, const String &path3, const String &path4);
		};
	}
}
