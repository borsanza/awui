#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui::Windows::Forms {
	class Panel : public Control {
	  public:
		Panel();
		virtual ~Panel() = default;

		const virtual awui::Drawing::Size GetMinimumSize() const;
	};
} // namespace awui::Windows::Forms
