#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Effects {
		class Effect;
	}

	namespace Windows {
		namespace Forms {
			class SliderBrowser : public Control {
				private:
					int m_margin;
					awui::Effects::Effect * m_effect;
					Control * m_lastControl;
					float m_lastTime;
					int m_initPos;
					int m_selected;

				public:
					SliderBrowser();
					virtual ~SliderBrowser();

					void SetMargin(int margin);

					virtual void OnTick(float deltaSeconds);

					Control * GetControlSelected() const;
			};
		}
	}
}
