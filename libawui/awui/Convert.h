#ifndef _AWUI_CONVERT_H
#define _AWUI_CONVERT_H

#include <awui/Object.h>

namespace awui {
	class Convert final : public Object {
		public:
			static String ToString(int value);
			static String ToString(float value);
			static String ToString(float value, int precision);
			static String ToString(unsigned char value);
			static String ToString(char value);
	};
}

#endif
