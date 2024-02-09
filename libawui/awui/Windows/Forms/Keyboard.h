#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Keyboard : public Control {
				public:
					Keyboard();
					virtual ~Keyboard();

					virtual bool IsClass(Classes objectClass) const override;
			};
		}
	}
}
