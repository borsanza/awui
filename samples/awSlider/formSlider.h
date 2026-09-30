#pragma once

#include <awui/UI/Form.h>

namespace awui::UI {
	class SliderBrowser;
}

class FormSlider : public awui::UI::Form {
  private:
	awui::UI::SliderBrowser *m_slider;

	void InitializeComponent();

  public:
	FormSlider();
	virtual ~FormSlider() = default;
};
