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

class FormArcade : public awui::Windows::Forms::Form {
	private:
		Button * buttonL;
		RemoteButtons::Enum buttonPressed;
		awui::Random rand;

		void InitializeComponent();

	public:
		FormArcade();
		virtual ~FormArcade();

		virtual bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
		virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);
		virtual void OnTick();
};
