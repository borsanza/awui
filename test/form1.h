// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Form.h>

namespace awui {
	namespace Collections {
		class ArrayList;
	}

	namespace Diagnostics {
		class Process;
	}

	namespace Effects {
		class Effect;
	}

	namespace Windows {
		namespace Forms {
			class Panel;
			class SplitContainer;
		}
	}
}

class Form1 : public awui::Windows::Forms::Form {
	private:
		awui::Windows::Forms::SplitContainer * _splitter;
		awui::Windows::Forms::Panel * _panel;

		awui::Collections::ArrayList * _buttons;
		awui::Collections::ArrayList * _effects;

		bool runMame;
		awui::Diagnostics::Process* mameProcess;

	public:
		Form1();
		virtual ~Form1();

	private:
		void InitializeComponent();
		void AddButtonEffect(awui::Effects::Effect * effect);

		virtual void OnTick();

		void CheckMame();
		void CheckGames();
};
