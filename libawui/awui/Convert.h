// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUICONVERT_H__
#define __AWUICONVERT_H__

#include <awui/Object.h>

namespace awui {
	class Convert final : public Object {
		public:
			static String ToString(int value);
			static String ToString(float value);
			static String ToString(float value, int precision);
			static String ToString(unsigned char value);
	};
}

#endif
