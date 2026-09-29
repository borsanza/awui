#pragma once

#include <awui/String.h>

namespace awui {
	class Environment {
	  public:
		enum class SpecialFolder {
			LocalApplicationData,
		};

		static String GetNewLine();

		static String GetFolderPath(SpecialFolder folder);
	};
} // namespace awui
