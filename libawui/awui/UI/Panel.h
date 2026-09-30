#pragma once

#include <awui/UI/Control.h>

namespace awui::UI {
	class Panel : public Control {
	  public:
		Panel();
		virtual ~Panel() = default;

		const virtual awui::Drawing::Size GetMinimumSize() const;
	};
} // namespace awui::UI
