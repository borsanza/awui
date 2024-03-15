#pragma once

#include <awui/Windows/Forms/Form.h>

namespace awui::Windows::Forms {
	class SliderBrowser;
}

class FormSlider : public awui::Windows::Forms::Form {
  private:
	awui::Windows::Forms::SliderBrowser *m_slider;

	void InitializeComponent();

  public:
	FormSlider();
	virtual ~FormSlider() = default;
};
