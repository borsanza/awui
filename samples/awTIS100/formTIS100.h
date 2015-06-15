#ifndef _FORMTIS100_H
#define _FORMTIS100_H

#include <awui/String.h>
#include <awui/Windows/Forms/Form.h>

using namespace awui::Windows::Forms;

class FormTIS100 : public awui::Windows::Forms::Form {
	private:
		void InitializeComponent();

	public:
		FormTIS100();
		virtual ~FormTIS100();

		virtual void OnTick();
};

#endif
