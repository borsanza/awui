// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Control.h>
#include <awui/Collections/SortedList.h>

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

class Test1 : public awui::Windows::Forms::Control {
	private:
		awui::Windows::Forms::SplitContainer * _splitter;

		awui::Collections::ArrayList * _buttons;
		awui::Collections::ArrayList * _effects;

		bool runMame;
		bool endMame;
		awui::Diagnostics::Process* mameProcess;
		awui::Collections::SortedList games;

	public:
		Test1();
		virtual ~Test1();

	private:
		void InitializeComponent();
		void AddButtonEffect(awui::Effects::Effect * effect, Control * control, int posy = -1);

		virtual void OnTick();

		void CheckMame();
		void CheckGames();
};
