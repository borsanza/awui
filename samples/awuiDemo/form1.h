#pragma once

#include <awui/Windows/Forms/Form.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Bitmap;
		}
	}
}

class Form1 : public awui::Windows::Forms::Form {
	private:
		awui::Windows::Forms::Bitmap * bitmap2;
	public:
		Form1();
		virtual ~Form1();

	private:
		void InitializeComponent();
};
