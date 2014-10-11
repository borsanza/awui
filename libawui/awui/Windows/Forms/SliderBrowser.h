#ifndef _AWUI_WINDOWS_FORMS_SLIDERBROWSER_H
#define _AWUI_WINDOWS_FORMS_SLIDERBROWSER_H

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class SliderBrowser : public Control {
				private:
					int _margin;
	
				public:
					SliderBrowser();
					virtual ~SliderBrowser();
					
					void SetMargin(int margin);
					
					virtual void OnTick();
			};
		}
	}
}

#endif