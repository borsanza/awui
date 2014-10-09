#ifndef _FORMMASTERGEAR_H
#define _FORMMASTERGEAR_H

#include <awui/String.h>
#include <awui/Windows/Emulators/MasterSystem.h>
#include <awui/Windows/Forms/Form.h>

using namespace awui::Windows::Forms;

class FormMasterGear : public awui::Windows::Forms::Form {
	private:
		void InitializeComponent();
		awui::Windows::Emulators::MasterSystem * _masterSystem;

		void AdjustSizeOfMasterSystem(awui::Windows::Emulators::MasterSystem * masterSystem);

	public:
		FormMasterGear();
		virtual ~FormMasterGear();

		void LoadRom(const awui::String file);
};

#endif
