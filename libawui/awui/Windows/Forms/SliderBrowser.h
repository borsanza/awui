#ifndef _AWUI_WINDOWS_FORMS_SLIDERBROWSER_H
#define _AWUI_WINDOWS_FORMS_SLIDERBROWSER_H

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Effects {
		class Effect;
	}

	namespace Windows {
		namespace Forms {
			class SliderBrowser : public Control {
				private:
					int _margin;
					awui::Effects::Effect * _effect;
					Control * _lastControl;
					float _lastTime;
					int _initPos;
					int _selected;
	
				public:
					SliderBrowser();
					virtual ~SliderBrowser();
					
					void SetMargin(int margin);
					
					virtual void OnTick();

					Control * GetControlSelected() const;
			};
		}
	}
}

#endif