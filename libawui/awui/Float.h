// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIFLOAT_H__
#define __AWUIFLOAT_H__

#include <awui/Object.h>

namespace awui {
	class Float final : public Object {
		private:
			float value;
		public:
			Float();
			Float(float value);
			virtual ~Float();

			virtual String ToString();
	};
}

#endif
