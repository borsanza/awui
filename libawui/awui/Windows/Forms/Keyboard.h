#pragma once

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
