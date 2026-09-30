#pragma once

#include <awui/UI/Control.h>

namespace awui {
	namespace Effects {
		class Effect;
	}

	namespace UI {
		class SliderBrowser : public Control {
		  private:
			int m_margin;
			awui::Effects::Effect *m_effect;
			Control *m_lastControl;
			float m_lastTime; // Segundos de la animación hacia el control seleccionado
			int m_initPos;
			int m_selected;

		  public:
			// Duración del desplazamiento hasta el control seleccionado (la de antes: 10 frames a 60 Hz)
			static constexpr float AnimationSeconds = 10.0f / 60.0f;

			SliderBrowser();
			virtual ~SliderBrowser();

			void SetMargin(int margin);

			virtual void OnTick(float deltaSeconds);

			Control *GetControlSelected() const;
		};
	} // namespace UI
} // namespace awui
