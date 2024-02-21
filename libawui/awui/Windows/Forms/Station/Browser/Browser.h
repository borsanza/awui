#pragma once

#include <awui/Windows/Forms/Gradient.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Station {
				namespace Browser {
					class Page;

					class Browser : public Control {
						private:
							Gradient _gradientUp;
							Gradient _gradientBottom;
							Gradient _gradientLeft;
							Gradient _gradientRight;
							Page * _page;

						public:
							Browser();
							virtual ~Browser();

							void SetPage(Page * page);

							virtual void OnTick(float deltaTime);
					};
				}
			}
		}
	}
}
