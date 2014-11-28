#ifndef _SAMPLES_AWCHIP8_FORM1_H
#define _SAMPLES_AWCHIP8_FORM1_H

#include <awui/String.h>
#include <awui/Windows/Forms/Form.h>

namespace awui {
	namespace Collections {
		class ArrayList;
	}

	namespace Windows {
		namespace Emulators {
			class Chip8;
		}

		namespace Forms {
			class SliderBrowser;
		}
	}
}

using namespace awui::Collections;
using namespace awui::Windows::Forms;

class Form1 : public awui::Windows::Forms::Form {
	private:
		SliderBrowser * _slider;
		ArrayList * _games;
		bool _invertedColors;

		void InitializeComponent();

		void AdjustSizeOfChip8(awui::Windows::Emulators::Chip8 * _chip8);

	public:
		Form1();
		virtual ~Form1();

		void LoadRom(const awui::String file);

		virtual void OnTick();
		virtual bool OnKeyPress(Keys::Enum key);
};

#endif
