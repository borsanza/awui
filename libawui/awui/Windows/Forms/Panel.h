#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Panel : public Control {
				public:
					Panel();
					virtual ~Panel();

					virtual int IsClass(Classes::Enum objectClass) const;

					const virtual awui::Drawing::Size GetMinimumSize() const;
			};
		}
	}
}
