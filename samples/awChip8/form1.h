#ifndef _SAMPLES_AWCHIP8_FORM1_H
#define _SAMPLES_AWCHIP8_FORM1_H

#include <awui/String.h>
#include <awui/Windows/Emulators/Chip8.h>
#include <awui/Windows/Forms/Form.h>

using namespace awui::Windows::Forms;

#define TOTALCHIP8 2

class Form1 : public awui::Windows::Forms::Form {
	private:
		void InitializeComponent();
		awui::Windows::Emulators::Chip8 * _chip8[TOTALCHIP8];

		void AdjustSizeOfChip8(awui::Windows::Emulators::Chip8 * _chip8);

	public:
		Form1();
		virtual ~Form1();

		void LoadRom(const awui::String file);

		virtual void OnTick();
};

#endif
