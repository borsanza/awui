#pragma once

#include <awui/Windows/Forms/Gradient.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Station {
				namespace Browser {
					class Page : public Control {
						private:

						public:
							Page();
							virtual ~Page();

							virtual void OnTick();
					};
				}
			}
		}
	}
}
