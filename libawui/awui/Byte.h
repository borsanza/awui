// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIBYTE_H__
#define __AWUIBYTE_H__

#include <awui/Object.h>

namespace awui {
	class Byte final : public Object {
	private:
		unsigned char value;
	public:
		Byte();
		Byte(unsigned char value);
		virtual ~Byte();

		virtual String ToString();
	};
}

#endif
