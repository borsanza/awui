// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIKEYBOARD_H__
#define __AWUIKEYBOARD_H__

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Keyboard : public Control {
				public:
					Keyboard();
					virtual ~Keyboard();

					virtual int IsClass(Classes::Enum objectClass) const;
			};
		}
	}
}

#endif
