// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Collections {
		class ArrayList;
	}

	namespace Effects {
		class Effect;
	}

	namespace Windows {
		namespace Forms {
			class SplitContainer;
		}
	}
}

class Test1 : public awui::Windows::Forms::Control {
	private:
		awui::Windows::Forms::SplitContainer * _splitter;

		awui::Collections::ArrayList * _buttons;
		awui::Collections::ArrayList * _effects;

	public:
		Test1();
		virtual ~Test1();

	private:
		void InitializeComponent();
		void AddButtonEffect(awui::Effects::Effect * effect, Control * control, int posy = -1);

		virtual void OnTick();
};
