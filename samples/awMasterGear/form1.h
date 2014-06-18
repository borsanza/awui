#ifndef _SAMPLES_MASTERGEAR_FORM1_H
#define _SAMPLES_MASTERGEAR_FORM1_H

#include <awui/String.h>
#include <awui/Windows/Emulators/MasterSystem.h>
#include <awui/Windows/Forms/Form.h>

using namespace awui::Windows::Forms;

class Form1 : public awui::Windows::Forms::Form {
	private:
		void InitializeComponent();
		awui::Windows::Emulators::MasterSystem * _masterSystem;

		void AdjustSizeOfMasterSystem(awui::Windows::Emulators::MasterSystem * masterSystem);

	public:
		Form1();
		virtual ~Form1();

		void LoadRom(const awui::String file);
};

#endif