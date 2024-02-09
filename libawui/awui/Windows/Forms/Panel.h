#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Panel : public Control {
				public:
					Panel();
					virtual ~Panel();

					virtual bool IsClass(Classes objectClass) const override;

					const virtual awui::Drawing::Size GetMinimumSize() const;
			};
		}
	}
}
