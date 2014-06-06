// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/String.h>
#include <awui/Windows/Emulators/Chip8.h>
#include <awui/Windows/Forms/Form.h>

using namespace awui::Windows::Forms;

class Form1 : public awui::Windows::Forms::Form {
	private:
		void InitializeComponent();
		awui::Windows::Emulators::Chip8 * _chip8;

	public:
		Form1();
		virtual ~Form1();

		void LoadRom(const awui::String file);
};