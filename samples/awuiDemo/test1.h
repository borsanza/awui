#pragma once

#include <awui/UI/Control.h>

#include <vector>

namespace awui {
	namespace Effects {
		class Effect;
	}

	namespace UI {
		class Button;
		class SplitContainer;
	}
} // namespace awui

class Test1 : public awui::UI::Control {
  private:
	awui::UI::SplitContainer *m_splitter;

	std::vector<awui::UI::Button *> m_buttons;
	std::vector<awui::Effects::Effect *> m_effects;

  public:
	Test1();
	virtual ~Test1() = default;

  private:
	void InitializeComponent();
	void AddButtonEffect(awui::Effects::Effect *effect, Control *control, int posy = -1);

	virtual void OnTick(float deltaSeconds) override;
};
