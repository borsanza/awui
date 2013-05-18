// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Form.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Bitmap;
			class MouseEventArgs;
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
		virtual void OnMouseMove(awui::Windows::Forms::MouseEventArgs* e);
};
