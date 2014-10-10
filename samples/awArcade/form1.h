// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Form.h>
#include <awui/Random.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Button;
		}
	}
}

using namespace awui::Windows::Forms;

class Form1 : public awui::Windows::Forms::Form {
	private:
		Button * buttonL;
		RemoteButtons::Enum buttonPressed;
		awui::Random rand;

		void InitializeComponent();

	public:
		Form1();
		virtual ~Form1();

		virtual bool OnRemoteKeyPress(RemoteButtons::Enum button);
		virtual bool OnRemoteKeyUp(RemoteButtons::Enum button);
		virtual void OnTick();
};
