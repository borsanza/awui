#pragma once

#include <awui/Collections/SortedList.h>
#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class ListBox;
		}
	}

	namespace Diagnostics {
		class Process;
	}
}

class Test2: public awui::Windows::Forms::Control {
	private:
		bool runMame;
		bool endMame;
//		awui::Diagnostics::Process* mameProcess;
		awui::Collections::SortedList games;
		awui::Windows::Forms::ListBox *listbox;

		void InitializeComponent();

	public:
		Test2();
		virtual ~Test2();

		virtual void OnTick();

		void CheckMame();
		void CheckGames();
};
