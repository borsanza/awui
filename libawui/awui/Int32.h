// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIINT32_H__
#define __AWUIINT32_H__

#include <awui/Object.h>

namespace awui {
	class Int32 final : public Object {
	private:
		int value;
	public:
		Int32();
		Int32(int value);
		virtual ~Int32();

		virtual String ToString();
	};
}

#endif
