// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Form.h>

using namespace awui::Windows::Forms;

namespace awui {
	namespace Windows {
		namespace Forms {
			class SliderBrowser;
		}
	}
}

class FormSlider : public awui::Windows::Forms::Form {
	private:
		SliderBrowser * _slider;

		void InitializeComponent();

	public:
		FormSlider();
		virtual ~FormSlider();
};