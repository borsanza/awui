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
							Gradient m_gradientUp;
							Gradient m_gradientBottom;
							Gradient m_gradientLeft;
							Gradient m_gradientRight;
							Page * m_page;

						public:
							Browser();
							virtual ~Browser();

							void SetPage(Page * page);

							virtual void OnTick(float deltaSeconds);
					};
				}
			}
		}
	}
}
