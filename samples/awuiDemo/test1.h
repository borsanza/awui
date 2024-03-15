#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Collections {
		class ArrayList;
	}

	namespace Effects {
		class Effect;
	}

	namespace Windows::Forms {
		class SplitContainer;
	}
} // namespace awui

class Test1 : public awui::Windows::Forms::Control {
  private:
	awui::Windows::Forms::SplitContainer *m_splitter;

	awui::Collections::ArrayList *m_buttons;
	awui::Collections::ArrayList *m_effects;

  public:
	Test1();
	virtual ~Test1() = default;

  private:
	void InitializeComponent();
	void AddButtonEffect(awui::Effects::Effect *effect, Control *control, int posy = -1);

	virtual void OnTick(float deltaSeconds) override;
};
