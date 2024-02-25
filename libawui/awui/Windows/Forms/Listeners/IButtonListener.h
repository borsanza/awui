
#pragma once

#include <awui/Object.h>

namespace awui::Windows::Forms::Listeners {
	class IButtonListener {
		public:
			virtual ~IButtonListener() = default;
			virtual void OnClick(Control* sender) = 0;
	};
}
